var path = require('path');
var os = require('os');
const EventEmitter = require('events').EventEmitter;
var Promise = require('bluebird');

platform = os.platform()
arch = os.arch()
_EXT = '.so'
if (platform == 'win32') {
	platform = 'windows'
	_EXT = '.dll'
}

var node_plugin = require('./bin/' + platform + '-' + arch)

// todo: add version
class Base_Plugin extends EventEmitter {
	constructor(name) {
		super();
		this.name = name;
		this.plugin_ = new node_plugin.Plugin(name + _EXT);
	}

	initialize(options, cb) {
		var dir = "";
		var opt = "";

		if (options) {
			if (options.plugin) {
				if (options.plugin.directory) {
					dir = options.plugin.directory;
				}
				delete options.plugin;
			}
			opt = JSON.stringify(options);
		}

		let self = this;
		let notify = (res) => {
			self.emit('notify', res);
		}
		this.plugin_.initialize(dir, opt, notify, cb);
	}

	terminate(cb) {
		this.plugin_.release(cb);
	}

	call(buf, cb) {
		this.plugin_.call(buf, cb);
	}

}


class Plugin extends EventEmitter {
	constructor(name) {
		super();
		this.plugin_ = new Base_Plugin(name);
		this.version_ = undefined;
		
		let self = this;
		this.plugin_.on('notify', buf => {
			self.emit('notify', buf);
		})
	}
	version() {
		return this.version_;
	}
	initialize(options) {
		let self = this;
		return new Promise(function (resolve, reject) {
			self.plugin_.initialize(options, (status, res, version) => {
				self.version_ = version;
				if (status == 0)
					resolve(res);
				else
					reject(res);
			})
		})
	}
	terminate() {
		let self = this;
		return new Promise(function (resolve, reject) {
			self.plugin_.terminate((status, res) => {
				if (status == 0)
					resolve(res);
				else
					reject(res);
			})
		})
	}
	call(buf) {
		let self = this;
		return new Promise(function (resolve, reject) {
			self.plugin_.call(buf, (res, status) => {
				if (status == 0)
					resolve(res);
				else
					reject(res);
			})
		})
	}
}

module.exports = {
	Plugin: Plugin
}
/*-----------------------------------------------------------------------------------------*/

// plugin = new Plugin('calc')
// plugin.initialize(options = {
// 	plugin: {
// 		directory: __dirname + '/bin'
// 	},
// 	user: 'xxxxxx'
// }, (status, msg) => {
// 	console.log('init status: ' + status);
// 	console.log(msg);
// 	if (status == 0) {
// 		var expr = Buffer.from("100+23", 'utf8');
// 		plugin.call(expr, (buf, status) => {
// 			console.log(expr.toString(), '=', buf.toString());
// 			console.log('call return status: ' + status);
// 			plugin.terminate((status, msg) => {
// 				console.log('terminate status: ' + status);
// 				console.log(msg);
// 			});
// 		});
// 	}
// 
// });
// plugin.on('notify', (buf) => {
// 	console.log('notify: ' + buf.toString());
// })
////-------------------------------------------------------------------------------------
// var expr = Buffer.from("100+23", 'utf8');
// plugin = new Plugin('calc')
// let options = {
// 	plugin: {
// 		directory: __dirname + '/bin'
// 	},
// 	user: 'xxxxxx'
// };
// plugin.initialize(options)
// 	.then(res => {
// 		console.log(res);
// 		return plugin.call(expr);
// 	})
// 	.then(res => {
// 		console.log(res.toString());
// 		return plugin.call(expr);
// 	})
// 	.then(res => {
// 		console.log(res.toString());
// 		return plugin.terminate();
// 	})
// 	.then(res => {
// 		console.log(res);
// 	})
// 	.catch(err => {
// 		console.log(err);
// 		plugin.terminate();
// 	})

// plugin.on('notify', (buf) => {
// 	console.log('notify: ' + buf.toString());
// })

