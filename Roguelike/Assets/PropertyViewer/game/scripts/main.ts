/// <reference path="_references.ts" />

interface EntityRef {
    id: Number;
    name: String;
}

class Entity implements EntityRef {
    id: number;
    name: string;
    components: string[];
    children: EntityRef[];

    static loadFromId(id: Number, callback: (entity: Entity) => void) {
        $.get('/game/api/entity/id/' + id, (data) => {
            callback(new Entity(data));
        });
    }

    constructor(jsonResponse: string) {
        this.copyFrom(jsonResponse);
    }

    copyFrom(data) {
        this.id = data.id;
        this.name = data.name;
        this.components = data.components;
        this.children = data.children;
    }

    refresh() {
        Entity.loadFromId(this.id, (entity) => {
            this.copyFrom(entity);
        });
    }
}

$(document).ready(() => {
    Entity.loadFromId(0, (entity) => {
        $('#droot').text(JSON.stringify(entity));
    });
});
