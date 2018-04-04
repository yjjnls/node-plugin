var path = require('path');
const chai = require('chai');
let expect = chai.expect,
    assert = chai.assert;
var Converter = require('./converter').Converter


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

