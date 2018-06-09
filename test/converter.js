var path = require('path');

var Plugin = require('../index').Plugin

class Converter {
    constructor(notify) {
        
        this.upper  = 0
        this.lower  = 0
        this.illegal= 0


        this.plugin_ = new Plugin('case-converter-plugin',
        path.dirname(Plugin.__file__), 
        (data,meta)=>{this.notification(data,meta)}  );

        
    }
    initialize(option){
        return this.plugin_.initialize(option);
    }

    terminate(){
        return this.plugin_.terminate()
    }

    convert(action,text){
        let self = this
        return new Promise(function (resolve, reject) {
            var meta = Buffer.from(action,'utf8')
            var data = Buffer.from(text,'utf8')
            self.plugin_.call(data,meta ).then(
                (res)=>{
                    var value = res.toString('utf8')
                    resolve(value);
                }
            ).catch( (err)=>{
                var msg = err.toString()
                reject(msg);
            })
		})
    }

    notification(data,meta){
        var type = meta.toString('utf8');
        var j={}
        if( data ){
            j = JSON.parse(data.toString('utf8'));
        }

        if( type == 'success'  ){
            this.upper = j.upper
            this.lower = j.lower
        } else if( type == 'illegal'){
            this.illegal = j.illegal;
        }
    }
    
}

module.exports = {
	Converter: Converter
}
