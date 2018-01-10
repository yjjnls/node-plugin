var Plugin = require('./node_plugin').Plugin
var obj = new Plugin('./bin/example.dll')
//
//var addon = require('./build/Release/node_plugin');
//
//console.log("~~~~~~~~~~");
//var obj = new addon.Plugin('e:/github.com/node-plugin/plugin/build/Debug/sample.dll');
//
//var   data = Uint8Array.from("1234");
//const param  = Buffer.from("123");//data.buffer);
//
//console.log("@",param)
//
//console.log("*************************");
//
//for( var i = 0;i<100;i++){
//	obj.call( param, function( buf){
//		console.log( buf.toString());
//
//	});
//}
//console.log("*************************");
//
////obj = null
//setTimeout( function(){
//	obj.release();
//	obj = null;
//	console.log("--- END ---");
//},1000);
