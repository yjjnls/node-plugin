'use strict'
const path = require('path')
const Package  = require('./conan').Package
const rmdirSync  = require('./conan').rmdirSync
const enums = require('./enums')
const Arch = enums.Arch
const Platform = enums.Platform
async function main(){
    var version = '0.5.6'
    var options ={
        bintray: 'bintray.com',
        subject: 'pluginx',
        repo: 'stable',
    }



    var pkg = new Package('plugin.node', version, options);
    await pkg.load();
    var ids = pkg.find_packages( Platform.current, Arch.current,'Release');
    if( ids < 0 ){
        return new Promise( (resolve,reject)=>{
            reject(`${Platform.current} ${Arch.current} package not found.`)
        })
    }
    await pkg.extract_package(ids[0], `./bin/${Arch.current}`,
        // file => path.basename(file.path) === 'plugin.node', //filter
        file => { //map
            file.path = path.basename(file.path)
            return file;
        });
    rmdirSync(pkg._cachedir);

}




main().then(val=>{
    console.log("plugin.node install done !");
    process.exit(0)
}).catch(err=>{
    console.error("plugin.node install failed:",err);
    process.exit(1)
})




