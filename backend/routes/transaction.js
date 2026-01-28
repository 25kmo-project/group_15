const express = require('express');
const router = express.Router();
const transaction = require('../models/transaction_model');

// Get all transactions
router.get('/', function(request, response) {
    transaction.getAll(function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

// Get a single transaction by its ID
router.get('/:id', function(request, response) {
    transaction.getById(request.params.id, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            // Returns the first (and only) result from the array
            response.json(dbResult[0]);
        }
    });
});

// Get all transactions for a specific account
// This is used to display the transaction history for one account
router.get('/account/:id', function(request, response) {
    transaction.getByAccountId(request.params.id, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

router.get('/card/:id', function(request, response) {
    transaction.getByCardId(request.params.id, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});
// Add a new transaction
router.post('/', function(request, response) {
    transaction.add(request.body, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

// Update a transaction
router.put('/:id', function(request, response) {
    transaction.update(request.params.id, request.body, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

// Delete a transaction
router.delete('/:id', function(request, response) {
    transaction.delete(request.params.id, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

// create a new withdrawal transaction
router.post('/withdraw', function(req, res) {
    transaction.withdraw(req.body, function(err, result) {
        if (err) {
            const clientErrors = ['INVALID_AMOUNT', 'NOT_FOUND', 'LOCKED', 'INSUFFICIENT_FUNDS'];
            const statusCode = clientErrors.includes(err.error) ? 400 : 500;
            return res.status(statusCode).json(err);
        }
        res.json({
            status: "SUCCESS",
            ...result,
            transaction_date: new Date()
        });
    });
});



module.exports = router;