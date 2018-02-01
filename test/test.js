var Plugin = require('../index').Plugin;

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
async function Initialize(arg) {
	await plugin.initialize(arg)
		.then(res => {
			console.log(res);
		})
		.catch(err => {
			console.log(err);
			throw new Error(err.toString());
		})
}


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
			console.log(err.toString());
			throw new Error(err.toString());
		})
}

async function Terminate() {
	await plugin.terminate()
		.then(res => {
			console.log(res);
		})
		.catch(err => {
			console.log(err.toString());
			throw new Error(err.toString());
		})
}

async function test(buf) {
	try {
		await Initialize(options);
		console.log('plugin version: ' + plugin.version());
		await call1(buf);
		await call2(buf);
		await Terminate();
	} catch (e) {
		console.log('-----------------error----------------');
		console.error(e.message);
		await Terminate();
	}
}

plugin.on('notify', (buf) => {
	console.log('notify: ' + buf.toString());
})
test(expr);