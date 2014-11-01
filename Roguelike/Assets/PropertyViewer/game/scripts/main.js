/// <reference path="_references.ts" />

var Entity = (function () {
    function Entity(jsonResponse) {
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
        this.components = data.components;
        this.children = data.children;
    };

    Entity.prototype.refresh = function () {
        var _this = this;
        Entity.loadFromId(this.id, function (entity) {
            _this.copyFrom(entity);
        });
    };
    return Entity;
})();

$(document).ready(function () {
    Entity.loadFromId(0, function (entity) {
        $('#droot').text(JSON.stringify(entity));
    });
});
