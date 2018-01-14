var Promise_plugin = require('../promise_plugin.js').Promise_plugin;
var Promise = require('bluebird');

const param = Buffer.from("123");//data.buffer);

async function call1(buf) {
    await Promise_plugin.callAsync(buf)
        .then((res) => {
            console.log('~~~~~~~~call1~~~~~~');
            console.log(res.toString());
            // promise_destroy();
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

call1(param);
// call2(param);
// test(param);