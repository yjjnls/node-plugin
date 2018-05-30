var path = require('path');
var os   = require('os');
var fs   = require("fs");

var Promise = require('bluebird');
const enums = require('./enums');
const Arch  = enums.Arch;
const DLExt = enums.DynamicLibraryExt;

var node_plugin = undefined;

class Plugin  {
	
	constructor(name, dir ,notify) {
		if(!node_plugin){
			var module = `./bin/${Arch.current}/plugin.node`;
			this.__addon__ = path.normalize( path.resolve(__dirname,path) )
			if (!path.existsSync(path)){
				throw Error(`${path} not exists.`)
			}
			node_plugin = require(`./bin/${Arch.current}/plugin.node`);
		}

		this.name    = name;
		if( notify == undefined ){
			if( dir == undefined ){
				dir = '';
			} else if ( typeof(dir) == 'function'){
				notify = dir
				dir = '';
			}
		}

		this.notify_ = notify;
		this.dir_    = dir
		this.version = null;
	}

	initialize(option) {
		let self = this;
		if( option == undefined )
		{
			option = null;
		}
		var filename = `${this.name}.${DLExt.current}`
		if( this.dir_){
			filename = `${this.dir_}/${this.name}.${DLExt.current}`
		}


		if (!fs.existsSync( filename) )
		{
			return new Promise(function (resolve, reject) {
				reject(`plugin<${filename}> not exists.`);
			})
		}

		self.plugin_ = new node_plugin.Plugin(path.basename(filename),self.dir_, self.notify_ )

		if( !self.plugin_.setup() ){
			return new Promise(function (resolve, reject) {
				reject(self.plugin_.error);
			})
		}
		this.version = self.plugin_.version;
		this.__file__ = path.resolve(filename);

		return new Promise(function (resolve, reject) {
			var text = JSON.stringify(option,'utf8');
			var opt = Buffer.from(text,'utf8');

			self.plugin_.initialize(opt, (status,res) => {
				if (status == 0)
					resolve(res);
				else
					reject(res);
			})
		})
	}

	terminate(callback) {
		let self = this;

		return new Promise(function (resolve, reject) {

			self.plugin_.release( (status,res) => {
				setTimeout(function() {
					self.plugin_.teardown()
				}, 0);

				if (status == 0)
				{
					resolve(res);
				}	
				else
					reject(res);
			})
		})
	}

	call(data,meta) {
		let self = this;
		if( meta == undefined ){
			meta = null;
		}

		if( data == undefined ){
			data = null;
		}

		return new Promise(function (resolve, reject) {
			self.plugin_.call(data, meta, (status,res) => {
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