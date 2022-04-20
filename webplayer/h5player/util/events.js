class EventEmitter{
    constructor() {
        this._events = Object.create(null);
    }

    addListener(type, handler) {
        (this._events[type] || (this._events[type] = [])).push(handler);
    }

    removeAllListeners(type, handler) {
        if (type === undefined) {
            this._events = []
            return
        }
        if(this._events[type]) {
            this._events[type].splice(this._events[type].indexOf(handler) >>> 0, 1)
        }
    }

    once(type, handler) {
        let fired = false;
        function magic() {
            this.off(type, handler);
            if(!fired) {
                fired = true;

                handler.apply(this, arguments);
            }
        }
        this.on(type, magic);
    }

    emit(type) {
        let payload = [].slice.call(arguments, 1);

        let array = this._events[type] ||[];
        for (let i = 0; i < array.length; i++) {
            let handler = this._events[type][i];
            handler.apply(this, payload);
        }
    }
}

// export default EventEmitter