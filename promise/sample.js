function Dog(name) {
    console.log('---------create dog---------');
    this.name = !name ? 'Tiger' : name;
}

Dog.prototype.bite = function (target, cb) {
    console.log(this.name + ' bite ' + target);
    if (target == 'YOU')
        cb(null, target);
    else
        throw new Error(target);
};

var Promise = require('bluebird');

// var d = Promise.promisifyAll(new Dog());
// d.biteAsync('hellokitty');

var someDog = new Dog("small");
var otherDog = new Dog("big");
var proDog = Promise.promisify(someDog.bite, { context: otherDog });
console.log('=============================================');
// proDog('YOU')
//     .then(function (target) {
//         console.log('then ' + target);
//         return proDog('YOU');
//     })
//     .then(function (target) {
//         console.log('then ' + target);
//     })
//     .catch(function (err) {
//         console.log(err.toString());
//     });

// proDog('xxx')
//     .then(function (target) {
//         console.log('then ' + target);
//     })
//     .catch(function (err) {
//         console.log(err.toString());
//     });

async function dog() {
    console.log('proDog(YOU)  1');
    await proDog('YOU')
        .then(function (target) {
            console.log('then ' + target);
            console.log('proDog(YOU)  2');
            return proDog('YOU');
        })
        .then(function (target) {
            console.log('then ' + target);
        })
        .catch(function (err) {
            console.log(err.toString());
        });
}

async function test(buf) {
    try {
        console.log('await dog(buf);');
        await dog(buf);
        // await promise_call(buf);
        console.log('************');
    } catch (e) {
        console.log('---------------------------------');
        console.error(e.message);
    }
}
test();