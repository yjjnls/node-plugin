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

	initialize(options, callback) {
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
		let notify = (data,meta) => {
			self.emit('notify', data,meta);
		}
		this.plugin_.initialize(dir, opt, notify, callback);
	}

	terminate(callback) {
		this.plugin_.release(callback);
	}

	call(data,meta, callback) {
		if( callback == undefined ){
			callback = meta;
			meta = undefined
		}
		this.plugin_.call(data,meta, cb);
	}

}


class Plugin extends EventEmitter {
	constructor(name) {
		super();
		this.plugin_ = new Base_Plugin(name);
		this.version_ = undefined;
		
		let self = this;
		this.plugin_.on('notify', (data,meta) => {
			self.emit('notify', data,meta);
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
	call(data,meta) {
		let self = this;
		return new Promise(function (resolve, reject) {
			self.plugin_.call(buf, meta,(res, status) => {
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



