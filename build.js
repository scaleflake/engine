/* jshint esversion: 6*/

const fs = require('fs');
const exec = require('child_process').exec;

let objs = '';

function clear() {
	let folders = [
		'bin',
		'obj'
	];

	function rmrf(i) {
		let child = exec(`powershell rm -force -r .\\${folders[i]}`);
		child.on('exit', () => {
			if (i < folders.length - 1) {
				rmrf(i + 1);
			} else {
				prepare();
			}
		});
	}

	rmrf(0);
}

function prepare() {
	let folders = [
		'bin',
		'obj',
		'obj\\Physics',
	];

	function mkdir(i) {
		let child = exec(`powershell mkdir .\\${folders[i]}`);
		child.on('exit', () => {
			if (i < folders.length - 1) {
				mkdir(i + 1);
			} else {
				compile();
			}
		});
	}

	mkdir(0);
}

function compile() {
	const compilingParams = '-m64 -fexceptions -Wall -Wno-unused-function -std=c++11';
	const include = '-Iinclude\\glew -Iinclude\\stbi -Iinclude\\glm -Iinclude\\glew -Iinclude\\glfw';

	let sourceFiles = [
		'Main', 'OpenGL', 'Windows',
		'Physics\\Algorithms',
		'Physics\\Body',
		'Physics\\Collider',
		'Physics\\Collision',
		'Physics\\Shape'
	];
	function compileSrcFile(i) {
		let a = sourceFiles[i];
		console.log(a);
		let child = exec(`powershell g++ ${compilingParams} ${include} -c src\\${a}.cpp -o obj\\${a}.o`);
		child.on('exit', () => {
			objs += `obj\\${a}.o `;
			if (i < sourceFiles.length - 1) {
				compileSrcFile(i + 1);
			} else {
				link();
			}
		});
	}
	compileSrcFile(0);
}

function link() {
	console.log('linking ...');

	const linkingParams = '-m64 -std=c++11 -lglew32s -lopengl32 -lglfw3 -lgdi32';
	const libs = '-Llibs\\glew -Llibs\\glfw';

	let child = exec(`powershell g++ ${libs} -o .\\bin\\Engine.exe ${objs} ${linkingParams}`);
	child.on('exit', () => {
		console.log('done.');
	});
}

clear();

// const target = process.argv[2];

// switch (target) {
// 	case undefined:
// 		{
// 			clear();
// 			break;	
// 		}
// 	case 'clear':
// 		{
// 			clear();
// 			break;
// 		}
// 	case 'prepare':
// 		{
// 			prepare();
// 			break;
// 		}
// }

