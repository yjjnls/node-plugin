var path = require('path');
var os = require('os');
platform = os.platform()
arch = os.arch()
ext='.so'
if( platform == 'win32' ){
	platform = 'windows'
	ext='.dll'
}

node_plugin=require('./bin/'+platform +'-' +arch)

example = path.join(__dirname,'/bin/example'+ext)
module.exports = {
	Plugin  : node_plugin.Plugin,
	EXAMPLE : example
}
