var path = require('path');
const chai = require('chai');
let expect = chai.expect,
    assert = chai.assert;
var Converter = require('./converter').Converter


////var convert = new plugin('calc','E:/github.com/node-plugin/c++/plugin/build/Debug/');
//console.log(Plugin.__file__)
//var convert = new Plugin('converter',path.dirname(Plugin.__file__));
//
//async function main(){
//        await convert.initialize()
//   
//        result = await convert.call( Buffer.from('AaBbCc','utf8'),Buffer.from('lower','utf8'));
//        console.log("Main end!",result.toString('utf8'))
//
//        convert.terminate();
//
//    
//}
//main().then( data=>{
//    console.log("SUCESS!")
//}).catch( err =>{
//    console.log("ERR!",err.toString('utf8'))
//})
//
describe('Node-Plugin', function () {

    describe('#basic', function () {
        it(`initialize and terminate`, async () => {
            var c = new Converter();
            await c.initialize();
            await c.terminate();
            
        });
    });

    describe('#interface', function () {
        let convert=null

        beforeEach(async () => {
            convert = new Converter();
            await convert.initialize()
        });

        afterEach(async () => {            
            await convert.terminate()
            convert = null
        });

        it(`call`, async () => {
            result = await convert.convert('upper','AaBbCc')
            assert.equal( result , 'AABBCC')
            result = await convert.convert('lower','AaBbCc')
            assert.equal( result , 'aabbcc')

        });

        it(`notify`, async () => {
            await convert.convert('upper','AaBbCc')
            assert.equal(convert.lower, 0 )

            await convert.convert('lower','AaBbCc')
            assert.equal(convert.illegal, 0 )
            assert.equal(convert.upper, 1 )



        });

    });

});

