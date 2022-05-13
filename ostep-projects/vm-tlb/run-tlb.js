#!/usr/bin/env node
const { execFile } = require("child_process");

run();

async function run() {
	let seed = 5;
	const multiplier = 10;
	const limit = seed * 1000000;
	const trials = 2;

	while (seed < limit) {
		try {
			const pagesToTouch = seed;
			seed = seed * multiplier;
			const result = await tlb(pagesToTouch, trials);
			console.log(result);
		} catch (err) {
			console.error(err);
		}
	}
}

function tlb(pages, trials) {
	const args = [pages.toString(), trials.toString()];
	return new Promise((resolve, reject) => {
		execFile("./tlb", args, (fileException, stdout, stderr) => {
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
