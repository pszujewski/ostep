#!/usr/bin/env node
const { execFile } = require("child_process");

function run() {
	let i = 1;
	const numberOfPagesSeed = 2;

	while (i < numberOfPagesSeed * 10001) {
		const numPages = (numberOfPagesSeed * i).toString();

		execFile("./tlb", [numPages], (_, stdout, stderr) => {
			console.log(stdout);
			console.log("\n");
		});

		i = i * 10;
	}
}

run();
