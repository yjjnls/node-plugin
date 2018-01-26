try {
	var node_plugin = require('node_plugin')
}
catch(e){
	var node_plugin = require('../index')
}

console.log(node_plugin.EXAMPLE)
var obj = new node_plugin.Plugin('example')

const param  = Buffer.from("123");//data.buffer);

console.log("*************************");

for( var i = 0;i<100;i++){
	obj.call( param, function( buf){
		console.log( buf.toString());

	});
}
console.log("*************************");

////obj = null
setTimeout( function(){
	obj.release();
	obj = null;
	console.log("--- END ---");
},500);
