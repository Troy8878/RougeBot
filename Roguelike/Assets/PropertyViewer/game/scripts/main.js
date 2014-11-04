/// <reference path="_references.ts" />
function isalnum(c) {
    return /^[0-9a-z]$/i.test(c);
}

function encodeStr(data) {
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

function decodeStr(data) {
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

function makeEntityButton(parent, label, id, destroyOld) {
    var button = document.createElement('button');
    button.textContent = label;

    $(button).click(function (e) {
        destroyOld();
        Entity.loadFromId(id, function (entity) {
            entity.build($('#droot').get(0));
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

var UnimplView = (function () {
    function UnimplView() {
    }
    UnimplView.prototype.build = function (parent) {
    };

    UnimplView.prototype.refresh = function () {
    };

    UnimplView.prototype.getBasePath = function () {
        return "";
    };
    return UnimplView;
})();

var BoolView = (function () {
    function BoolView(owner, property) {
        this.owner = owner;
        this.property = property;
    }
    BoolView.prototype.build = function (parent) {
        var _this = this;
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

        $(this.checkbox).change(function () {
            _this.data.value = _this.checkbox.checked;
            _this.upload();
        });

        parent.appendChild(this.dataNode);
        this.refresh();
    };

    BoolView.prototype.refresh = function () {
        var _this = this;
        $.get(this.getBasePath(), function (data) {
            _this.data = data;
            _this.displayValues();
        });
    };

    BoolView.prototype.displayValues = function () {
        this.checkbox.checked = this.data.value;
    };

    BoolView.prototype.upload = function () {
        var _this = this;
        var data = JSON.stringify({ type: "bool", value: this.data.value });
        var path = this.getSetPath() + "/" + encodeStr(encodeStr(data));
        $.get(path, function () {
            _this.refresh();
        });
    };

    BoolView.prototype.getBasePath = function () {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    };

    BoolView.prototype.getSetPath = function () {
        return this.owner.getBasePath() + "/set/" + this.property.id;
    };
    return BoolView;
})();

var FloatView = (function () {
    function FloatView(owner, property) {
        this.owner = owner;
        this.property = property;
    }
    FloatView.prototype.build = function (parent) {
        var _this = this;
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
        $(this.textbox).keyup(function () {
            var newval = parseFloat(_this.textbox.value);
            if (!isNaN(newval)) {
                _this.data.value = newval;
                _this.upload();
            }
        });

        parent.appendChild(this.dataNode);
        this.refresh();
    };

    FloatView.prototype.refresh = function () {
        var _this = this;
        $.get(this.getBasePath(), function (data) {
            _this.data = data;
            _this.displayValues();
        });
    };

    FloatView.prototype.displayValues = function () {
        this.textbox.value = this.data.value.toString();
    };

    FloatView.prototype.upload = function () {
        var data = JSON.stringify({ type: "float", value: this.data.value });
        var path = this.getSetPath() + "/" + encodeStr(encodeStr(data));
        $.get(path);
    };

    FloatView.prototype.getBasePath = function () {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    };

    FloatView.prototype.getSetPath = function () {
        return this.owner.getBasePath() + "/set/" + this.property.id;
    };
    return FloatView;
})();

var IntView = (function () {
    function IntView(owner, property) {
        this.owner = owner;
        this.property = property;
    }
    IntView.prototype.build = function (parent) {
        var _this = this;
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
        $(this.textbox).keyup(function () {
            var newval = parseInt(_this.textbox.value);
            if (!isNaN(newval)) {
                _this.data.value = newval;
                _this.upload();
            }
        });

        parent.appendChild(this.dataNode);
        this.refresh();
    };

    IntView.prototype.refresh = function () {
        var _this = this;
        $.get(this.getBasePath(), function (data) {
            _this.data = data;
            _this.displayValues();
        });
    };

    IntView.prototype.displayValues = function () {
        this.textbox.value = this.data.value.toString();
    };

    IntView.prototype.upload = function () {
        var data = JSON.stringify({ type: "int", value: this.data.value });
        var path = this.getSetPath() + "/" + encodeStr(encodeStr(data));
        $.get(path);
    };

    IntView.prototype.getBasePath = function () {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    };

    IntView.prototype.getSetPath = function () {
        return this.owner.getBasePath() + "/set/" + this.property.id;
    };
    return IntView;
})();

var StringView = (function () {
    function StringView(owner, property) {
        this.owner = owner;
        this.property = property;
    }
    StringView.prototype.build = function (parent) {
        var _this = this;
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
        $(this.textbox).keyup(function () {
            _this.data.value = _this.textbox.value;
            _this.upload();
        });

        parent.appendChild(this.dataNode);
        this.refresh();
    };

    StringView.prototype.refresh = function () {
        var _this = this;
        $.get(this.getBasePath(), function (data) {
            _this.data = data;
            _this.displayValues();
        });
    };

    StringView.prototype.displayValues = function () {
        this.textbox.value = this.data.value;
    };

    StringView.prototype.upload = function () {
        var data = JSON.stringify({ type: "string", value: this.data.value });
        var path = this.getSetPath() + "/" + encodeStr(encodeStr(data));
        $.get(path, function () {
            //this.refresh();
        });
    };

    StringView.prototype.getBasePath = function () {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    };

    StringView.prototype.getSetPath = function () {
        return this.owner.getBasePath() + "/set/" + this.property.id;
    };
    return StringView;
})();

var VectorView = (function () {
    function VectorView(owner, property) {
        this.owner = owner;
        this.property = property;
        this.x = new FloatView(this, { id: "x", type: "float", can_set: true });
        this.y = new FloatView(this, { id: "y", type: "float", can_set: true });
        this.z = new FloatView(this, { id: "z", type: "float", can_set: true });
        this.w = new FloatView(this, { id: "w", type: "float", can_set: true });
    }
    VectorView.prototype.build = function (parent) {
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
    };

    VectorView.prototype.refresh = function () {
        this.x.refresh();
        this.y.refresh();
        this.z.refresh();
        this.w.refresh();
    };

    VectorView.prototype.getBasePath = function () {
        return this.owner.getBasePath() + "/get/" + this.property.id;
    };
    return VectorView;
})();

function makePropertyView(owner, property) {
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
            return new VectorView(owner, property);
    }

    return new UnimplView();
}

var ComponentView = (function () {
    function ComponentView(owner, name) {
        this.owner = owner;
        this.name = name;
        this.visible = false;
        this.loaded = false;
        this.rootNode = null;
        this.dataNode = null;
        this.expandButton = null;
        this.contentNode = null;
        this.loadingNode = null;
    }
    ComponentView.prototype.getBasePath = function () {
        return this.owner.getBasePath() + "/component/" + this.name;
    };

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

    ComponentView.prototype.buildViewers = function () {
        this.propertyViews = [];
        for (var i = 0; i < this.properties.length; ++i) {
            var property = this.properties[i];
            var viewer = makePropertyView(this, property);
            viewer.build(this.contentNode);
            this.propertyViews[i] = viewer;
        }
    };

    ComponentView.prototype.loadData = function () {
        var _this = this;
        $.get(this.getBasePath(), function (data) {
            if (_this.loadingNode != null) {
                _this.loadingNode.parentElement.removeChild(_this.loadingNode);
                _this.loadingNode = null;
            }

            _this.contentNode = document.createElement('div');
            _this.dataNode.appendChild(_this.contentNode);

            _this.properties = data.properties;
            _this.buildViewers();
        });
    };

    ComponentView.prototype.refresh = function () {
        this.dataNode.removeChild(this.contentNode);
        this.loadData();
    };

    ComponentView.prototype.displayValues = function () {
    };

    ComponentView.prototype.showLoading = function () {
        var loading = document.createElement('span');
        loading.textContent = "Loading...";

        this.loadingNode = loading;
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

    Entity.prototype.getBasePath = function () {
        return "/game/api/entity/id/" + this.id;
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

    Entity.prototype.build = function (parent) {
        this.rootNode = document.createElement('div');
        this.rootNode.classList.add("entity");

        this.createButtons();

        this.viewers.id = new ValueView().build(this.rootNode).label("ID");
        this.viewers.name = new ValueView().build(this.rootNode).label("Name");

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
        entity.build($('#droot').get(0));
    });
});
