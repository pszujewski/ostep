#!/usr/bin/env node
const { execFile } = require("child_process");

function run() {
	let i = 1;
	const numberOfPagesSeed = 2;

	while (i < numberOfPagesSeed * 10001) {
		const numPages = (numberOfPagesSeed * i).toString();
		console.log("numPages => ", numPages);

		execFile("./tlb", [numPages], (fileException, stdout, stderr) => {
			console.log(stdout);
			console.log(stderr);
			if (fileException) {
				console.log(fileException);
			}
		});

		i = i * 10;
	}
}

run();
