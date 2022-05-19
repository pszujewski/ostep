#!/usr/bin/env node
const { plot } = require("nodeplotlib");
const { execFile } = require("child_process");

run();

async function run() {
	let seed = 5;
	const multiplier = 10; // If I make this "2" and decrease the "limit", page access doesn't really seem to grow due to compiler optimizations?
	const limit = 5000000;
	const trials = 3;
	const data = { x: [], y: [], type: "scatter" };

	while (seed <= limit) {
		try {
			const pagesToTouch = seed;
			seed = seed * multiplier;
			const result = await tlb(pagesToTouch, trials);
			const { ns, pages } = processResult(result);
			console.log(`pages, ${pages}, ns: ${ns}`);
			data.x.push(pages);
			data.y.push(ns);
		} catch (err) {
			console.error(err);
			break;
		}
	}

	plot([data]);
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

function processResult(result) {
	const tokens = result.split(",");
	return {
		ns: tokens[0],
		pages: tokens[1],
	};
}
