var express = require('express');
var router = express.Router();
const https = require('https');


// =========================
// GET /currency/latest
// =========================
// Palauttaa viimeisimmän työpäivän kurssit
router.get('/latest', function(req, res) {

    const url = "https://api.frankfurter.dev/v1/latest?base=EUR&symbols=USD,GBP";

    https.get(url, (apiRes) => {

        let data = "";

        apiRes.on("data", chunk => {
            data += chunk;
        });

        apiRes.on("end", () => {
            try {
                const json = JSON.parse(data);

                res.json({
                    base: json.base,
                    date: json.date,
                    USD: json.rates.USD,
                    GBP: json.rates.GBP
                });

            } catch (err) {
                res.status(500).json({ error: "Invalid API response" });
            }
        });

    }).on("error", err => {
        res.status(500).json({ error: "Currency API request failed" });
    });
});


// =========================
// GET /currency/change
// =========================
// Palauttaa:
// - viimeisin kurssi
// - muutos edelliseen työpäivään
// - prosenttimuutos
router.get('/change', function(req, res) {

    // haetaan ~10 päivän data, jotta saadaan 2 viimeistä työpäivää
    const end = new Date();
    const start = new Date(end);
    start.setDate(end.getDate() - 10);

    const format = (d) => d.toISOString().slice(0, 10);

    const url = `https://api.frankfurter.dev/v1/${format(start)}..${format(end)}?base=EUR&symbols=USD,GBP`;

    https.get(url, (apiRes) => {

        let data = "";

        apiRes.on("data", chunk => {
            data += chunk;
        });

        apiRes.on("end", () => {
            try {
                const json = JSON.parse(data);
                const rates = json.rates;

                const dates = Object.keys(rates).sort();

                if (dates.length < 2) {
                    return res.status(500).json({ error: "Not enough rate data" });
                }

                const prevDate = dates[dates.length - 2];
                const lastDate = dates[dates.length - 1];

                const prev = rates[prevDate];
                const last = rates[lastDate];

                function calc(symbol) {
                    const rLast = Number(last[symbol]);
                    const rPrev = Number(prev[symbol]);

                    const abs = rLast - rPrev;
                    const pct = rPrev !== 0 ? (abs / rPrev) * 100 : 0;

                    return {
                        rate: rLast,
                        abs: Number(abs.toFixed(6)),
                        pct: Number(pct.toFixed(2))
                    };
                }

                res.json({
                    base: json.base || "EUR",
                    lastDate,
                    prevDate,
                    USD: calc("USD"),
                    GBP: calc("GBP")
                });

            } catch (err) {
                res.status(500).json({ error: "Invalid API response" });
            }
        });

    }).on("error", err => {
        res.status(500).json({ error: "Currency API request failed" });
    });
});


module.exports = router;
