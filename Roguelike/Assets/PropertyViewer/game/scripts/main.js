/// <reference path="_references.ts" />

var currentEntity;

function makeEntityButton(parent, label, id, destroyOld) {
    var button = document.createElement('button');
    button.textContent = label;

    $(button).click(function (e) {
        destroyOld();
        Entity.loadFromId(id, function (entity) {
            currentEntity = entity;
            entity.buildView($('#droot').get(0));
        });
    });

    parent.appendChild(button);
}

var ValueView = (function () {
    function ValueView() {
    }
    ValueView.prototype.build = function (parent) {
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
    };

    ValueView.prototype.label = function (value) {
        this.labelNode.textContent = value;
        return this;
    };

    ValueView.prototype.value = function (value) {
        this.valueNode.textContent = value.toString();
        return this;
    };

    ValueView.prototype.destroy = function () {
        this.rootNode.parentElement.removeChild(this.rootNode);
        return this;
    };
    return ValueView;
})();

var ComponentView = (function () {
    function ComponentView() {
    }
    ComponentView.prototype.build = function (parent, properties) {
        this.properties = properties;
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add('component');

        for (var property in properties) {
            this.propertyViews[property] = new ValueView().build(this.rootNode).label(property);
        }

        parent.appendChild(this.rootNode);
    };
    return ComponentView;
})();

var Entity = (function () {
    function Entity(jsonResponse) {
        this.viewers = {};
        this.copyFrom(jsonResponse);
    }
    Entity.loadFromId = function (id, callback) {
        $.get('/game/api/entity/id/' + id, function (data) {
            callback(new Entity(data));
        });
    };

    Entity.prototype.copyFrom = function (data) {
        this.id = data.id;
        this.name = data.name;
        this.parent = data.parent;
        this.components = data.components;
        this.children = data.children;
    };

    Entity.prototype.refresh = function () {
        var _this = this;
        Entity.loadFromId(this.id, function (entity) {
            _this.copyFrom(entity);
            _this.displayValues();
        });
    };

    Entity.prototype.createButtons = function () {
        var _this = this;
        var buttons = document.createElement('div');

        var refreshButton = document.createElement('button');
        refreshButton.textContent = "Refresh";
        $(refreshButton).click(function (e) {
            _this.refresh();
        });
        buttons.appendChild(refreshButton);

        if (this.id != 0) {
            makeEntityButton(buttons, "View Parent", this.parent.id, this.destroy);
        }

        this.rootNode.appendChild(buttons);
    };

    Entity.prototype.buildView = function (parent) {
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add("entity");

        this.createButtons();

        this.viewers.id = new ValueView().build(this.rootNode).label("ID");
        this.viewers.name = new ValueView().build(this.rootNode).label("Name");
        this.viewers.components = new ValueView().build(this.rootNode).label("Components");

        parent.appendChild(this.rootNode);

        this.displayValues();
    };

    Entity.prototype.displayValues = function () {
        this.viewers.id.value(this.id);
        this.viewers.name.value(this.name);
        this.viewers.components.value(this.components.join(", "));
    };

    Entity.prototype.destroy = function () {
        this.rootNode.parentElement.removeChild(this.rootNode);
    };
    return Entity;
})();

$(document).ready(function () {
    Entity.loadFromId(0, function (entity) {
        currentEntity = entity;
        entity.buildView($('#droot').get(0));
    });
});
