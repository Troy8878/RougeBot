/// <reference path="_references.ts" />

function isalnum(c: string) {
    return /^[0-9a-z]$/i.test(c);
}

function encodeStr(data: string) {
    var out = "";
    for (var i = 0; i < data.length; ++i) {
        var c = data.charAt(i);
        if (isalnum(c)) {
            out += c;
        } else {
            var code = c.charCodeAt(0).toString(16);
            if (code.length == 1)
                code = "0" + code;

            out += "%" + code;
        }
    }
    return out;
}

function decodeStr(data: string) {
    var out = "";
    for (var i = 0; i < data.length; ++i) {
        var c = data.charAt(i);
        if (c == '%') {
            var code = data.substr(i + 1, 2);
            i += 2;
            out += String.fromCharCode(parseInt(code, 16));
        } else {
            out += c;
        }
    }
    return out;
}

interface Viewable {
    refresh(): void;
    build(parent: HTMLElement): void;
    getBasePath(): string;
}

interface EntityRef {
    id: number;
    name: string;
}

function makeEntityButton(
    parent: HTMLElement, label: string,
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

function makeZombieButton(
    parent: HTMLElement, label: string,
    ent: Entity, destroyOld: () => void) {

    var button = document.createElement('button');
    button.textContent = label;

    $(button).click((e) => {
        $.get(ent.getBasePath() + "/zombify");

        destroyOld();
        Entity.loadFromId(ent.parent.id, (entity) => {
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

interface Property {
    id: string;
    type: string;
    can_set: boolean;
}

interface BoolProp {
    type: string;
    value: boolean;
}

interface NumProp {
    type: string;
    value: number;
}

interface StringProp {
    type: string;
    value: string;
}

class UnimplView implements Viewable {
    build(parent: HTMLElement) {
    }

    refresh() {
    }

    getBasePath() {
        return "";
    }
}

class BoolView implements Viewable {
    dataNode: HTMLElement;
    checkbox: HTMLInputElement;
    data: BoolProp;

    constructor(public owner: Viewable, public property: Property) {
    }

    build(parent: HTMLElement) {
        this.dataNode = document.createElement('div');
        this.dataNode.classList.add('property');
        this.dataNode.classList.add('bool-view');

        var label = document.createElement('div');
        label.classList.add("boolview-label");
        label.textContent = this.property.id;
        this.dataNode.appendChild(label);

        this.checkbox = document.createElement('input');
        this.checkbox.type = 'checkbox';
        this.dataNode.appendChild(this.checkbox);

        $(this.checkbox).change(() => {
            this.data.value = this.checkbox.checked;
            this.upload();
        });

        parent.appendChild(this.dataNode);
        this.refresh();
    }

    refresh() {
        $.get(this.getBasePath(), (data: BoolProp) => {
            this.data = data;
            this.displayValues();
        });
    }

    displayValues() {
        this.checkbox.checked = this.data.value;
    }

    upload() {
        var data = JSON.stringify({ type: "bool", value: this.data.value });
        var path = this.getSetPath() + "/" + encodeStr(encodeStr(data));
        $.get(path, () => {
            this.refresh();
        });
    }

    getBasePath() {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    }

    getSetPath() {
        return this.owner.getBasePath() + "/set/" + this.property.id;
    }
}

class FloatView implements Viewable {
    dataNode: HTMLElement;
    textbox: HTMLInputElement;
    data: NumProp;

    constructor(public owner: Viewable, public property: Property) {
    }

    build(parent: HTMLElement) {
        this.dataNode = document.createElement('div');
        this.dataNode.classList.add('property');
        this.dataNode.classList.add('float-view');

        var label = document.createElement('div');
        label.classList.add("property-label");
        label.textContent = this.property.id;
        this.dataNode.appendChild(label);

        this.textbox = document.createElement('input');
        this.textbox.type = 'text';
        this.dataNode.appendChild(this.textbox);
        $(this.textbox).keyup(() => {
            var newval = parseFloat(this.textbox.value);
            if (!isNaN(newval)) {
                this.data.value = newval;
                this.upload();
            }
        });

        parent.appendChild(this.dataNode);
        this.refresh();
    }

    refresh() {
        $.get(this.getBasePath(), (data: NumProp) => {
            this.data = data;
            this.displayValues();
        });
    }

    displayValues() {
        this.textbox.value = this.data.value.toString();
    }

    upload() {
        var data = JSON.stringify({ type: "float", value: this.data.value });
        var path = this.getSetPath() + "/" + encodeStr(encodeStr(data));
        $.get(path);
    }

    getBasePath() {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    }

    getSetPath() {
        return this.owner.getBasePath() + "/set/" + this.property.id;
    }
}

class IntView implements Viewable {
    dataNode: HTMLElement;
    textbox: HTMLInputElement;
    data: NumProp;

    constructor(public owner: Viewable, public property: Property) {
    }

    build(parent: HTMLElement) {
        this.dataNode = document.createElement('div');
        this.dataNode.classList.add('property');
        this.dataNode.classList.add('int-view');

        var label = document.createElement('div');
        label.classList.add("property-label");
        label.textContent = this.property.id;
        this.dataNode.appendChild(label);

        this.textbox = document.createElement('input');
        this.textbox.type = 'text';
        this.dataNode.appendChild(this.textbox);
        $(this.textbox).keyup(() => {
            var newval = parseInt(this.textbox.value);
            if (!isNaN(newval)) {
                this.data.value = newval;
                this.upload();
            }
        });

        parent.appendChild(this.dataNode);
        this.refresh();
    }

    refresh() {
        $.get(this.getBasePath(), (data: NumProp) => {
            this.data = data;
            this.displayValues();
        });
    }

    displayValues() {
        this.textbox.value = this.data.value.toString();
    }

    upload() {
        var data = JSON.stringify({ type: "int", value: this.data.value });
        var path = this.getSetPath() + "/" + encodeStr(encodeStr(data));
        $.get(path);
    }

    getBasePath() {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    }

    getSetPath() {
        return this.owner.getBasePath() + "/set/" + this.property.id;
    }
}

class StringView implements Viewable {
    dataNode: HTMLElement;
    textbox: HTMLInputElement;
    data: StringProp;

    constructor(public owner: Viewable, public property: Property) {
    }

    build(parent: HTMLElement) {
        this.dataNode = document.createElement('div');
        this.dataNode.classList.add('property');
        this.dataNode.classList.add('string-view');

        var label = document.createElement('div');
        label.classList.add("property-label");
        label.textContent = this.property.id;
        this.dataNode.appendChild(label);

        this.textbox = document.createElement('input');
        this.textbox.type = 'text';
        this.dataNode.appendChild(this.textbox);
        $(this.textbox).keyup(() => {
            this.data.value = this.textbox.value;
            this.upload();
        });

        parent.appendChild(this.dataNode);
        this.refresh();
    }

    refresh() {
        $.get(this.getBasePath(), (data: StringProp) => {
            this.data = data;
            this.displayValues();
        });
    }

    displayValues() {
        this.textbox.value = this.data.value;
    }

    upload() {
        var data = JSON.stringify({ type: "string", value: this.data.value });
        var path = this.getSetPath() + "/" + encodeStr(encodeStr(data));
        $.get(path, () => {
            //this.refresh();
        });
    }

    getBasePath() {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    }

    getSetPath() {
        return this.owner.getBasePath() + "/set/" + this.property.id;
    }
}

class VectorView implements Viewable {

    dataNode: HTMLElement;
    x: FloatView;
    y: FloatView;
    z: FloatView;
    w: FloatView;

    constructor(public owner: Viewable, public property: Property) {
        this.x = new FloatView(this, { id: "x", type: "float", can_set: true });
        this.y = new FloatView(this, { id: "y", type: "float", can_set: true });
        this.z = new FloatView(this, { id: "z", type: "float", can_set: true });
        this.w = new FloatView(this, { id: "w", type: "float", can_set: true });
    }

    build(parent: HTMLElement) {
        this.dataNode = document.createElement('div');
        this.dataNode.classList.add('property');
        this.dataNode.classList.add('vector-view');
        parent.appendChild(this.dataNode);

        var label = document.createElement('div');
        label.classList.add('property-label');
        label.textContent = this.property.id;
        this.dataNode.appendChild(label);

        this.x.build(this.dataNode);
        this.y.build(this.dataNode);
        this.z.build(this.dataNode);
        this.w.build(this.dataNode);
    }

    refresh() {
        this.x.refresh();
        this.y.refresh();
        this.z.refresh();
        this.w.refresh();
    }

    getBasePath() {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    }
}

function makePropertyView(owner: Viewable, property: Property): Viewable {
    switch (property.type) {
        case "bool":
            return new BoolView(owner, property);
        case "float":
            return new FloatView(owner, property);
        case "int":
            return new IntView(owner, property);
        case "string":
            return new StringView(owner, property);
        case "vector":
            return new VectorView(owner, property);
        case "color":
            return new VectorView(owner, property); // TODO: Make a real color view
    }

    return new UnimplView();
}

class ComponentView implements Viewable {
    visible: boolean = false;
    loaded: boolean = false;
    rootNode: HTMLElement = null;
    dataNode: HTMLElement = null;
    expandButton: HTMLElement = null;
    contentNode: HTMLElement = null;
    loadingNode: HTMLElement = null;

    properties: Property[];
    propertyViews: Viewable[];

    constructor(public owner: Entity, public name: string) {

    }

    getBasePath() {
        return this.owner.getBasePath() + "/component/" + this.name;
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

    buildViewers() {
        this.propertyViews = [];
        for (var i = 0; i < this.properties.length; ++i) {
            var property = this.properties[i];
            var viewer = makePropertyView(this, property);
            viewer.build(this.contentNode);
            this.propertyViews[i] = viewer;
        }
    }

    loadData() {
        $.get(this.getBasePath(), (data) => {
            if (this.loadingNode != null) {
                this.loadingNode.parentElement.removeChild(this.loadingNode);
                this.loadingNode = null;
            }

            this.contentNode = document.createElement('div');
            this.dataNode.appendChild(this.contentNode);

            this.properties = data.properties;
            this.buildViewers();
        });
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

        this.loadingNode = loading;
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

    getBasePath() {
        return "/game/api/entity/id/" + this.id;
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
            makeZombieButton(
                buttons, "Zombify!",
                this, () => this.destroy());
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
