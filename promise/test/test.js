var promise_call = require('../promise_plugin.js').promise_call;
var plugin_destroy = require('../promise_plugin.js').plugin_destroy;

const param = Buffer.from("123");
async function call1(buf) {
    await promise_call(buf)
        .then((res) => {
            console.log('~~~~~~~~call1~~~~~~');
            console.log(res.toString());
        })
        .catch((err) => {
            console.log('-------err-----');
            console.error(err.toString());
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

        })
        .catch((err) => {
            console.log('-------err-----');
            console.error(err.toString());
        });
}

async function test(buf) {
    try {
        await call1(buf);
        await call2(buf);
        console.log('******end******');
        plugin_destroy();
    } catch (e) {
        console.log('---------------------------------');
        console.error(e.message);
    }
}

test(param);