//
//
//
"use strict"
const rest = require('restler');
const os=require('os');
const path = require('path')
const assert = require('assert')
const fs = require('fs')
const hasha = require('hasha');
const download = require('download');
const ini = require('ini');
const decompress = require('decompress');
const enums = require('./enums')
const Arch = enums.Arch
const Platform = enums.Platform
const DLExt = enums.DynamicLibraryExt

const _OPTIONS={
    repo:'stable',
    subject:'pluginx',
    bintray:'bintray.com',
    cachedir:'~/.conanjs'
}

function abspath( p ){

    if (p.startsWith('~')){
        if( Platform.current == Platform.WINDOWS ){
            p = path.join(process.env.USERPROFILE,p.slice(1))
        }
    }
    return path.normalize( path.resolve( p ) )
}

        //var files = await this.files();
class Package {

    constructor(name,version,options=_OPTIONS)
    {
        options       = options ||{}
        this.name     = name
        this.version  = version
        this._files   = null
        this._repo    = options.repo    || _OPTIONS.repo   
        this._subject = options.subject || _OPTIONS.subject
        this._bintray = options.bintray || _OPTIONS.bintray
        this._cachedir = abspath( options.cachedir || _OPTIONS.cachedir )
        this._packages = undefined

    }

    // return package id
    // compiler is dict compiler={name:'Visual Studio',version:'15',runtime:'MD'}
    find_packages(platform,arch,build_type,compiler){
        var ids=[]
        for(var id in this._packages ){
            var prop = this._packages[id]
            
            if( !prop) continue;
            
            if( prop.os != platform ) continue;
            if( prop.arch != arch ) continue;
            if( prop.build_type != build_type ) continue;
            
            if( compiler ){
                if( compiler.name && compiler.name != prop.compiler) continue
                if( compiler.version && compiler.name != prop.compiler_name) continue
                if( compiler.runtime && compiler.runtime != prop.compiler_runtime) continue
            }
            
            ids.push( id)
        }
        return ids;
    }
    
    async load(){
        
        await this._load_files_info();
        
        await this._load_packages_info();

    }



    async extract_package(id,dst='.',filter_func,map_func){
        var prop = this._packages[id];
        var tarball = await this._cache( prop.files['conan_package.tgz'])
        return new Promise( (resolve, reject) => {
            var options={}
            if( filter_func ){
                options.filter = filter_func
            }
            if( map_func){
                options.map = map_func
            }
            decompress(tarball, dst, options).then(files => {
                resolve(files);
            }).catch(err=>{
                reject(err);
            });
        });
    }

    /**
     * 
     */
    _bintray_get_files(){
        let self = this
        //GET /packages/:subject/:repo/:package/files[?include_unpublished=0/1]
        var url = `https://api.${this._bintray}/packages/${this._subject}/${this._repo}/${self.name}:${this._subject}/files`
        return new Promise( (resolve, reject)=>{
            if (!self._files){
                rest.get(url).on( 'complete', (res) =>{
                    if (res instanceof Error) {
                        return reject( res);                        
                    } else {
                        self._files = res;
                        return resolve( self._files);
                    }
                }).on('fail',(data,res)=>{
                    reject(data);
                });
            } else {
                return resolve(self._files)
            }
        });
    }

    async _load_files_info(){
        if (this._packages){
            return this._packages;
        }
        this._packages={}
        var files = await this._bintray_get_files();

        files.forEach((it)=>{
            var name = it['name']
            
            var pit = it['path'].split('/')
            
            assert( pit[0] == this._subject);
            assert( pit[1] == this.name);
            // assert( pit[2] == this.version);
            // assert( pit[3] == this._repo);
            if( pit[2] == this.version && pit[4] == 'package'){
                assert( name == pit[6]);
                var id = pit[5]
                var pkg = this._packages[id] || {files:{}}
                pkg.files[name] = it
                this._packages[id] = pkg;
            }
        });
    }

    async _load_packages_info(){
        
        for(var id in this._packages ){
            var prop = this._packages[id]
            for(var file in prop.files){
                if (file == 'conaninfo.txt' ){
                    var filename = await this._cache(prop.files[file]);
                    var conaninfo = ini.parse(fs.readFileSync(filename, 'utf-8'))

                    prop.os   = conaninfo.settings.os;
                    prop.arch = conaninfo.settings.arch;
                    prop.build_type = conaninfo.settings.build_type;
                    prop.compiler   = conaninfo.settings.compiler;
                    prop.compiler_runtime = conaninfo.settings['compiler.runtime'];
                    prop.compiler_version = conaninfo.settings['compiler.version'];
                }
            }
        }
    }

    _conaninfo_file_parse( prop ){
        this._cache(prop);
    }

    _cachepath( rpath){
        return path.resolve(this._cachedir,this._bintray,rpath);
    }

    async _cache(prop, retry = 5){
        var filename = this._cachepath( prop.path );
        var url = `https://dl.${this._bintray}/${this._subject}/${this._repo}/${prop.path}`
        var success = false;
        for(var i=0; i < retry +1; i++){
            if( fs.existsSync( filename )){
                var sha1 = await hasha.fromFile(filename, {algorithm: 'SHA1'})
                if (sha1 == prop['sha1']){
                    success = true;
                    break;
                }
            }
            
            await download(url, path.dirname(filename));
            var sha1 = await hasha.fromFile(filename, {algorithm: 'SHA1'})
            if (sha1 == prop['sha1']){
                success = true;
                break;
            }    
        }
        return new Promise( (resolve, reject)=>{
            if( success){
                resolve(filename);
            } else {
                reject(`download <${url}> failed.`)
            }
        });
    }

}

async function main(){
    var pkg = new Package('plugin.node','0.3.0', {
        repo :'testing'
    });

    try{

        await pkg.load();
        var ids = pkg.find_packages('Windows','x86_64','Release');
        await pkg.extract_package(ids[0],'x',null,file=>{
            file.path = `${Arch.current}/${path.basename(file.path)}`
            return file;
        });
    } catch(err){
        console.error("err->",err);
    }
}

/*
main().then(val=>{
    console.log("DONE!");
}).catch(err=>{
    console.log("Failed:",err);
})
*/


module.exports = {
    Package: Package
}