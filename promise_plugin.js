'use strict'

var Promise = require('bluebird');
var origin_Plugin = require('./index.js').Plugin;

class Promise_plugin {
    constructor(name) {
        this.plugin_ = new origin_Plugin(name);
    }
    initialize(options, notify) {
        this.plugin_.initialize(options, notify);
    }
    terminate(cb) {
        this.plugin_.terminate(cb);
    }
    call(buf) {
        let self = this;
        return new Promise(function (resolve, reject) {
            self.plugin_.call(buf, (res, status) => {
                if (status==0)
                    resolve(res);
                else
                    reject(res);
            })
        })
    }
}

module.exports = {
    Plugin: Promise_plugin
}



