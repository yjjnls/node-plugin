'use strict'

var Promise = require('bluebird');

var node_plugin;
try {
    node_plugin = require('node_plugin')
}
catch (e) {
    node_plugin = require('../index')
}
let plugin = new node_plugin.Plugin(node_plugin.EXAMPLE);

function plugin_destroy() {
    plugin.release();
}
let promise_call = function (buf) {
    return new Promise(function (resolve, reject) {
        plugin.call(buf, res=>{
            resolve(res);
        })
    })
};

module.exports = {
    promise_call,
    plugin_destroy
}

