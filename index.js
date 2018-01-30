var path = require('path');
var os = require('os');
platform = os.platform()
arch = os.arch()
_EXT='.so'
if( platform == 'win32' ){
	platform = 'windows'
	_EXT='.dll'
}

var node_plugin=require('./bin/'+platform +'-' +arch)


class Plugin {
    constructor( name ) {
		this.name = name;
		this.plugin_ = new node_plugin.Plugin(name +_EXT)
		
    }

    initialize(options,notify) {
		var dir = "";
		var opt = "";

        if( options ){
			if( options.plugin ) {
				if( options.plugin.directory ){
					dir = options.plugin.directory
				}
				delete options.plugin;
			}
			opt = JSON.stringify(options);
	
		}

		this.plugin_.initialize(dir,opt,notify);
	}

	terminate(done){
		this.plugin_.release(done);
	}

	call(buf,cb){
		this.plugin_.call(buf,cb);
	}

}



//example = path.join(__dirname,'/bin/calc'+_EXT)
module.exports = {
	Plugin  : Plugin
}
/*-----------------------------------------------------------------------------------------*/
// function notify(buf){
// 	console.log(buf.toString());
// }

// plugin = new Plugin('calc')
// plugin.initialize(options={
//     plugin:{
//         directory: __dirname +'/bin'
//     },
//     user: 'xxxxxx'
// },notify);

// var expr = Buffer.from( "100+23", 'utf8' );
// plugin.call(expr, (buf,status) =>{
// 	console.log(expr.toString() ,'=',buf.toString());
// 	console.log(status)
// 	plugin.terminate(()=>{
// 		console.log(">>>> Termated <<<<");
// 	});
// });

// // setTimeout(()=>{
// // 	plugin.terminate(()=>{
// // 		console.log(">>>> Termated <<<<");
// // 	});
// // },1000);