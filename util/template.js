var fs = require('fs');

var arguments = process.argv.splice(2);

if (arguments[0])
    dst_dir = arguments[0];
else
    dst_dir = '.';

var src = 'c++/addon/node_plugin_interface.h';
var dst = dst_dir + '/node_plugin_interface.h';

function copy(src, dst) {
    fs.createReadStream(src).pipe(fs.createWriteStream(dst));
}

copy(src, dst);