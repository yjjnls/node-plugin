var promise_call = require('../promise_plugin.js').promise_call;
var promise_destroy = require('../promise_plugin.js').plugin_destroy;
var Promise = require('bluebird');

const param = Buffer.from("123");//data.buffer);

// promise_call(param)
//     .then((res) => {
//         console.log(res.toString());
//         return promise_call(param);
//     })
//     .then((res) => {
//         console.log(res.toString());
//         promise_destroy();
//     })
//     .catch((err) => {
//         console.log('-------err-----');
//         console.error(err.toString());
//         promise_destroy();
//     });

async function call1(buf) {
    await promise_call(buf)
        .then((res) => {
            console.log('~~~~~~~~call1~~~~~~');
            console.log(res.toString());
            promise_destroy();
        })
        .catch((err) => {
            console.log('-------err-----');
            console.error(err.toString());
            // return Promise.reject();
        });
}

async function call2(buf) {
    await promise_call(buf)
        .then((res) => {
            console.log('~~~~~~~~call2~~~~~~');
            console.log(res.toString());
            return promise_call(buf);
        })
        .then((res) => {
            console.log(res.toString());
            // promise_destroy();
            // return Promise.resolve();
        })
        .catch((err) => {
            console.log('-------err-----');
            console.error(err.toString());
            // return Promise.reject();
        });
}

async function test(buf) {
    try {
        // await call1(buf);
        await call2(buf);
        // await promise_call(buf);
        console.log('************');
        // promise_destroy();
    } catch (e) {
        console.log('---------------------------------');
        console.error(e.message);
        // promise_destroy();
    }
}

// call1(param);
// call2(param);
test(param);