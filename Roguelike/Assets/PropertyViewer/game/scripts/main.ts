/// <reference path="_references.ts" />

interface Viewable {
    refresh(): void;
    buildView(parent: HTMLElement): void;
}

interface EntityRef {
    id: number;
    name: string;
}

var currentEntity: Entity;

function makeEntityButton(parent: HTMLElement, label: string,
                          id: number, destroyOld: () => void) {
    var button = document.createElement('button');
    button.textContent = label;

    $(button).click((e) => {
        destroyOld();
        Entity.loadFromId(id, (entity) => {
            currentEntity = entity;
            entity.buildView($('#droot').get(0));
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
    rootNode: HTMLElement;
    properties: string[];
    propertyViews: any;

    build(parent: HTMLElement, properties: string[]) {
        this.properties = properties;
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add('component');

        for (var property in properties) {
            this.propertyViews[property] = new ValueView()
                .build(this.rootNode)
                .label(property);
        }
        
        parent.appendChild(this.rootNode);
    }
}

class Entity implements EntityRef, Viewable {
    id: number;
    name: string;
    parent: EntityRef;
    components: string[];
    children: EntityRef[];
    
    rootNode: HTMLElement;
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
                this.parent.id, this.destroy);
        }

        this.rootNode.appendChild(buttons);
    }

    buildView(parent: HTMLElement) {
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add("entity");

        this.createButtons();

        this.viewers.id = new ValueView()
            .build(this.rootNode)
            .label("ID");
        this.viewers.name = new ValueView()
            .build(this.rootNode)
            .label("Name");
        this.viewers.components = new ValueView()
            .build(this.rootNode)
            .label("Components");

        parent.appendChild(this.rootNode);

        this.displayValues();
    }

    displayValues() {
        this.viewers.id.value(this.id);
        this.viewers.name.value(this.name);
        this.viewers.components.value(this.components.join(", "));
    }

    destroy() {
        this.rootNode.parentElement.removeChild(this.rootNode);
    }
}

$(document).ready(() => {
    Entity.loadFromId(0, (entity) => {
        currentEntity = entity;
        entity.buildView($('#droot').get(0));
    });
});
