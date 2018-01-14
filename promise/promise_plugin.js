'use strict'

var Promise = require('bluebird');

var node_plugin;
try {
    node_plugin = require('node_plugin')
}
catch (e) {
    node_plugin = require('../index')
}

class promise_plugin {
    constructor() {
        this.obj = new node_plugin.Plugin(node_plugin.EXAMPLE);
    }
    destroy() {
        this.obj.release();
        this.obj = null;
    }
    call(buf, cb) {
        this.obj.call(buf, res => {
            cb(null, res);
        })
    }
}
let plugin = new promise_plugin();

let promise_call = Promise.promisify(plugin.call, { context: plugin });

function plugin_destroy() {
    plugin.destroy();
}
module.exports = {
    promise_call,
    plugin_destroy
}


// function Promise_plugin() {
//     return Promise.promisifyAll(new promise_plugin());
// }
// module.exports = {
//     Promise_plugin
// }
// const param = Buffer.from("123");//data.buffer);
// let p_plugin = Promise_plugin();

// p_plugin.callAsync(param)
//     .then((res) => {
//         console.log('-------then-----');
//         console.log(res.toString());
//         p_plugin.destroyAsync();
//     })
//     .catch((err) => {
//         console.log('-------err-----');
//         console.error(err.toString());
//         p_plugin.destroyAsync();

//     });

// promise_call(param)
//     .then((res) => {
//         console.log('-------then-----');
//         console.log(res.toString());
//         plugin.destroy();
//     })
//     .catch((err) => {
//         console.log('-------err-----');
//         console.error(err.toString());
//         plugin.destroy();
//     });

