class LocalStorage {
    constructor() {
        this.storage = {};
    }

    getItem(key) {
        return this.storage[key] || null;
    }

    setItem(key, value) {
        this.storage[key] = value;
    }

    removeItem(key) {
        delete this.storage[key];
    }

    clear() {
        this.storage = {};
    }

    key(index) {
        const keys = Object.keys(this.storage);
        return index < keys.length ? keys[index] : null;
    }

    get length() {
        return Object.keys(this.storage).length;
    }
}

localStorage = new LocalStorage();
