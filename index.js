

var Plugin = require('./lib/plugin').Plugin
var Conan  = require('./lib/conan')
var enums  = require('./lib/enums')
module.exports = {
	Plugin: Plugin,
	Conan:  Conan,
	enums:  enums
}

//var path = require('path');
//var os = require('os');
//var fs= require("fs")
//
//var Promise = require('bluebird');
//
//platform = os.platform()
//arch = os.arch()
//_EXT = '.so'
//if (platform == 'win32') {
//	platform = 'win'
//	_EXT = '.dll'
//}
//var node_plugin = require(`./bin/${platform}/${arch}/plugin.node`);
//
//class Plugin  {
//	
//	constructor(name, dir ,notify) {		
//		
//		this.name    = name;
//		if( notify == undefined ){
//			if( dir == undefined ){
//				dir = '';
//			} else if ( typeof(dir) == 'function'){
//				notify = dir
//				dir = '';
//			}
//		}
//
//		this.notify_ = notify;
//		this.dir_    = dir
//		this.version = null;
//	}
//
//	initialize(option) {
//		let self = this;
//		if( option == undefined )
//		{
//			option = null;
//		}
//		var filename = `${this.name}${_EXT}`
//		if( this.dir_){
//			filename = `${this.dir_}/${this.name}${_EXT}`
//		}
//
//
//		if (!fs.existsSync( filename) )
//		{
//			return new Promise(function (resolve, reject) {
//				reject(`plugin not exists, basename=${self.name}${_EXT} dir=${self.dir_}`);
//			})
//		}
//
//		self.plugin_ = new node_plugin.Plugin(`${self.name}${_EXT}`,self.dir_, self.notify_ )
//
//		if( !self.plugin_.setup() ){
//			return new Promise(function (resolve, reject) {
//				reject(self.plugin_.error);
//			})
//		}
//		this.version = self.plugin_.version;
//
//		return new Promise(function (resolve, reject) {
//			var text = JSON.stringify(option,'utf8');
//			var opt = Buffer.from(text,'utf8');
//
//			self.plugin_.initialize(opt, (status,res) => {
//				if (status == 0)
//					resolve(res);
//				else
//					reject(res);
//			})
//		})
//	}
//
//	terminate(callback) {
//		let self = this;
//
//		return new Promise(function (resolve, reject) {
//
//			self.plugin_.release( (status,res) => {
//				setTimeout(function() {
//					self.plugin_.teardown()
//				}, 0);
//
//				if (status == 0)
//				{
//					resolve(res);
//				}	
//				else
//					reject(res);
//			})
//		})
//	}
//
//	call(data,meta) {
//		let self = this;
//		if( meta == undefined ){
//			meta = null;
//		}
//
//		if( data == undefined ){
//			data = null;
//		}
//
//		return new Promise(function (resolve, reject) {
//			self.plugin_.call(data, meta, (status,res) => {
//				if (status == 0)
//					resolve(res);
//				else
//					reject(res);
//			})
//		})
//	}
//}
//
//Plugin.__file__=`${__dirname}/bin/${platform}/${arch}/plugin.node`
//
//module.exports = {
//	Plugin: Plugin,
//	DYNAMIC_MODULE_SUFFIX : _EXT
//}



