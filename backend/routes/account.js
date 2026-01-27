const express = require('express');
const router = express.Router();
const account = require('../models/account_model');

// GET all accounts
router.get('/', (req, res) => {
    account.getAll((err, result) => {
        if (err) return res.status(500).json(err);
        res.json(result);
    });
});

// GET account by ID
router.get('/:id', (req, res) => {
    account.getOne(req.params.id, (err, result) => {
        if (err) return res.status(500).json(err);
        res.json(result);
    });
});

// POST create new account
router.post('/', (req, res) => {
    account.add(req.body, (err, result) => {
        if (err) return res.status(500).json(err);
        res.status(201).json({ insertId: result.insertId });
    });
});

// PUT update account
router.put('/:id', (req, res) => {
    account.update(req.body, req.params.id, (err, result) => {
        if (err) return res.status(500).json(err);
        res.json(result);
    });
});

// DELETE account
router.delete('/:id', (req, res) => {
    account.delete(req.params.id, (err, result) => {
        if (err) return res.status(500).json(err);
        res.json(result);
    });
});

module.exports = router;
