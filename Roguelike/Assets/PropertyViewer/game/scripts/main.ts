/// <reference path="_references.ts" />

interface Viewable {
    refresh(): void;
    build(parent: HTMLElement): void;
}

interface EntityRef {
    id: number;
    name: string;
}

function makeEntityButton(parent: HTMLElement, label: string,
                          id: number, destroyOld: () => void) {
    var button = document.createElement('button');
    button.textContent = label;

    $(button).click((e) => {
        destroyOld();
        Entity.loadFromId(id, (entity) => {
            entity.build($('#droot').get(0));
        });
    });

    parent.appendChild(button);
}

class ValueView {
    rootNode: HTMLElement;
    labelNode: HTMLElement;
    valueNode: HTMLElement;

    build(parent: HTMLElement) {
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add('property');

        this.labelNode = document.createElement('div');
        this.labelNode.classList.add('property-label');
        this.rootNode.appendChild(this.labelNode);

        this.valueNode = document.createElement('div');
        this.valueNode.classList.add('property-value');
        this.rootNode.appendChild(this.valueNode);

        parent.appendChild(this.rootNode);
        return this;
    }

    label(value: string) {
        this.labelNode.textContent = value;
        return this;
    }

    value(value: any) {
        this.valueNode.textContent = value.toString();
        return this;
    }

    destroy() {
        this.rootNode.parentElement.removeChild(this.rootNode);
        return this;
    }
}

class ComponentView {
    visible: boolean = false;
    loaded: boolean = false;
    rootNode: HTMLElement = null;
    dataNode: HTMLElement = null;
    expandButton: HTMLElement = null;

    contentNode: HTMLElement = null;

    constructor(public owner: Entity, public name: string) {

    }

    makeComponentBar(): HTMLElement {
        var componentBar = document.createElement('div');
        componentBar.classList.add('component-header');

        var name = document.createElement('span');
        name.textContent = this.name;
        componentBar.appendChild(name);

        this.expandButton = document.createElement('button');
        this.expandButton.textContent = "+";
        componentBar.appendChild(this.expandButton);

        $(this.expandButton).click((e) => {
            this.toggleView();
        });

        return componentBar;
    }

    build(parent: HTMLElement) {
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add('component');

        this.rootNode.appendChild(this.makeComponentBar());

        this.dataNode = document.createElement('div');
        this.dataNode.classList.add('component-body');
        $(this.dataNode).hide();
        this.rootNode.appendChild(this.dataNode);

        parent.appendChild(this.rootNode);
    }

    loadData() {
        // TODO: Load the component data
    }

    refresh() {
        this.dataNode.removeChild(this.contentNode);
        this.loadData();
    }

    displayValues() {

    }

    showLoading() {
        var loading = document.createElement('span');
        loading.textContent = "Loading...";

        this.dataNode.appendChild(loading);
    }

    show() {
        if (this.visible)
            return;

        this.visible = true;
        $(this.dataNode).show();
        this.expandButton.textContent = "-";

        if (this.loaded) {
            return;
        }

        this.loaded = true;
        this.showLoading();
        this.loadData();
    }

    hide() {
        if (!this.visible)
            return;

        this.visible = false;
        this.expandButton.textContent = "+";
        $(this.dataNode).hide();
    }

    toggleView() {
        if (this.visible)
            this.hide();
        else
            this.show();
    }
}

class Entity implements EntityRef, Viewable {
    id: number;
    name: string;
    parent: EntityRef;
    components: string[];
    children: EntityRef[];
    
    rootNode: HTMLElement;
    componentsNode: HTMLElement;
    childrenNode: HTMLElement;
    viewers: any;
    
    static loadFromId(id: Number, callback: (entity: Entity) => void) {
        $.get('/game/api/entity/id/' + id, (data) => {
            callback(new Entity(data));
        });
    }

    constructor(jsonResponse: string) {
        this.viewers = {};
        this.copyFrom(jsonResponse);
    }

    copyFrom(data) {
        this.id = data.id;
        this.name = data.name;
        this.parent = data.parent;
        this.components = data.components;
        this.children = data.children;
    }
    
    refresh() {
        this.rootNode.removeChild(this.componentsNode);
        this.componentsNode = document.createElement('div');
        this.rootNode.appendChild(this.componentsNode);

        this.rootNode.removeChild(this.childrenNode);
        this.childrenNode = document.createElement('div');
        this.rootNode.appendChild(this.childrenNode);

        Entity.loadFromId(this.id, (entity) => {
            this.copyFrom(entity);
            this.displayValues();
        });
    }

    createButtons() {
        var buttons = document.createElement('div');

        var refreshButton = document.createElement('button');
        refreshButton.textContent = "Refresh";
        $(refreshButton).click((e) => {
            this.refresh();
        });
        buttons.appendChild(refreshButton);

        if (this.id != 0) {
            makeEntityButton(
                buttons, "View Parent",
                this.parent.id, () => this.destroy());
        }

        this.rootNode.appendChild(buttons);
    }

    build(parent: HTMLElement) {
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add("entity");

        this.createButtons();

        this.viewers.id = new ValueView()
            .build(this.rootNode)
            .label("ID");
        this.viewers.name = new ValueView()
            .build(this.rootNode)
            .label("Name");

        this.componentsNode = document.createElement('div');
        this.rootNode.appendChild(this.componentsNode);

        this.childrenNode = document.createElement('div');
        this.rootNode.appendChild(this.childrenNode);

        parent.appendChild(this.rootNode);

        this.displayValues();
    }

    displayValues() {
        this.viewers.id.value(this.id);
        this.viewers.name.value(this.name);

        for (var i = 0; i < this.components.length; ++i) {
            var component = this.components[i];
            new ComponentView(this, component).build(this.componentsNode);
        }

        for (var i = 0; i < this.children.length; ++i) {
            var child = this.children[i];
            makeEntityButton(
                this.childrenNode,
                "ID: " + child.id + ", Name: " + child.name,
                child.id, () => this.destroy());
        }
    }

    destroy() {
        this.rootNode.parentElement.removeChild(this.rootNode);
    }
}

$(document).ready(() => {
    Entity.loadFromId(0, (entity) => {
        entity.build($('#droot').get(0));
    });
});
