var Plugin = require('../promise_plugin').Plugin;

function notify(buf) {
	console.log(buf.toString());
}

let plugin = new Plugin('calc');
let options = {
	plugin: {
		directory: __dirname + '/../bin'
	},
	user: 'xxxxxx'
}
plugin.initialize(options, notify);

const expr = Buffer.from("100+23", 'utf8');
async function call1(buf) {
	await plugin.call(buf)
		.then(res => {
			console.log(buf.toString(), '=', res.toString());
		})
		.catch(err => {
			console.log(buf.toString());
		})
}

async function call2(buf) {
	await plugin.call(buf)
		.then(res => {
			console.log(buf.toString(), '=', res.toString());
			return plugin.call(buf);
		})
		.then(res => {
			console.log(buf.toString(), '=', res.toString());
		})
		.catch(err => {
			console.log(res.toString());
		})
}

async function test(buf) {
	try {
		await call1(buf);
		await call2(buf);
		console.log('******end******');
		plugin.terminate(() => {
			console.log(">>>> Terminated <<<<");
		});
	} catch (e) {
		console.log('---------------------------------');
		console.error(e.message);
	}
}

test(expr);