/// <reference path="_references.ts" />

function makeEntityButton(parent, label, id, destroyOld) {
    var button = document.createElement('button');
    button.textContent = label;

    $(button).click(function (e) {
        destroyOld();
        Entity.loadFromId(id, function (entity) {
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
    function ComponentView(owner, name) {
        this.owner = owner;
        this.name = name;
        this.visible = false;
        this.loaded = false;
        this.rootNode = null;
        this.dataNode = null;
        this.expandButton = null;
    }
    ComponentView.prototype.makeComponentBar = function () {
        var _this = this;
        var componentBar = document.createElement('div');
        componentBar.classList.add('component-header');

        var name = document.createElement('span');
        name.textContent = this.name;
        componentBar.appendChild(name);

        this.expandButton = document.createElement('button');
        this.expandButton.textContent = "+";
        componentBar.appendChild(this.expandButton);

        $(this.expandButton).click(function (e) {
            _this.toggleView();
        });

        return componentBar;
    };

    ComponentView.prototype.build = function (parent) {
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add('component');

        this.rootNode.appendChild(this.makeComponentBar());

        this.dataNode = document.createElement('div');
        this.dataNode.classList.add('component-body');
        $(this.dataNode).hide();
        this.rootNode.appendChild(this.dataNode);

        parent.appendChild(this.rootNode);
    };

    ComponentView.prototype.loadData = function () {
        // TODO: Load the component data
    };

    ComponentView.prototype.showLoading = function () {
        var loading = document.createElement('span');
        loading.textContent = "Loading...";

        this.dataNode.appendChild(loading);
    };

    ComponentView.prototype.show = function () {
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
    };

    ComponentView.prototype.hide = function () {
        if (!this.visible)
            return;

        this.visible = false;
        this.expandButton.textContent = "+";
        $(this.dataNode).hide();
    };

    ComponentView.prototype.toggleView = function () {
        if (this.visible)
            this.hide();
        else
            this.show();
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
        this.rootNode.removeChild(this.componentsNode);
        this.componentsNode = document.createElement('div');
        this.rootNode.appendChild(this.componentsNode);

        this.rootNode.removeChild(this.childrenNode);
        this.childrenNode = document.createElement('div');
        this.rootNode.appendChild(this.childrenNode);

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
            makeEntityButton(buttons, "View Parent", this.parent.id, function () {
                return _this.destroy();
            });
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

        this.componentsNode = document.createElement('div');
        this.rootNode.appendChild(this.componentsNode);

        this.childrenNode = document.createElement('div');
        this.rootNode.appendChild(this.childrenNode);

        parent.appendChild(this.rootNode);

        this.displayValues();
    };

    Entity.prototype.displayValues = function () {
        var _this = this;
        this.viewers.id.value(this.id);
        this.viewers.name.value(this.name);
        this.viewers.components.value(this.components.join(", "));

        for (var i = 0; i < this.components.length; ++i) {
            var component = this.components[i];
            new ComponentView(this, component).build(this.componentsNode);
        }

        for (var i = 0; i < this.children.length; ++i) {
            var child = this.children[i];
            makeEntityButton(this.childrenNode, "ID: " + child.id + ", Name: " + child.name, child.id, function () {
                return _this.destroy();
            });
        }
    };

    Entity.prototype.destroy = function () {
        this.rootNode.parentElement.removeChild(this.rootNode);
    };
    return Entity;
})();

$(document).ready(function () {
    Entity.loadFromId(0, function (entity) {
        entity.buildView($('#droot').get(0));
    });
});
