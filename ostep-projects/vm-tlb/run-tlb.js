#!/usr/bin/env node
const { execFile } = require("child_process");

run();

async function run() {
	let seed = 5;
	const multiplier = 10;
	const limit = seed * 1000000;

	while (seed < limit) {
		try {
			const arg = seed;
			seed = seed * multiplier;
			const result = await tlb(arg);
			console.log(result);
		} catch (err) {
			console.error(err);
		}
	}
}

function tlb(arg) {
	return new Promise((resolve, reject) => {
		execFile("./tlb", [arg.toString()], (fileException, stdout, stderr) => {
			if (stderr.length > 0) {
				reject(stderr);
			} else if (fileException) {
				reject(fileException);
			} else {
				resolve(stdout);
			}
		});
	});
}
