const express = require('express');
const router = express.Router();
const transaction = require('../models/transaction_model');
const authenticateToken = require('../middleware/auth');

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
router.get('/id/:id',authenticateToken, function(request, response) {
    transaction.getById(request.params.id, function(err, dbResult) {
        if (err) return response.status(500).json(err);
        if (!dbResult ||dbResult.length === 0) {
            return response.status(404).json({ error: "Transaction not found" });
        }
            // Returns the first (and only) result from the array
            response.json(dbResult[0]);
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
router.post('/withdraw',authenticateToken, function(req, res) {
    const { account_id, card_id, amount } = req.body;
    if (req.user.card_id != req.body.card_id) {
            return res.status(403).json({ error: "Security breach: You cannot access other cards" });
      }
    transaction.withdraw({ account_id, card_id, amount }, function(err, result) {
        
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

// Get paginated transaction history for an account and card
router.get('/history', authenticateToken,function(request, response) {
    const data = {
        account_id: parseInt(request.query.account_id),
        card_id: parseInt(request.query.card_id),
        page: parseInt(request.query.page) || 1
    };

    if (isNaN(data.account_id) || isNaN(data.card_id)) {
        return response.status(400).json({ error: 'Valid IDs required' });
    }
    if (request.user.card_id != data.card_id) {
        return response.status(403).json({ error: "Access denied" });
    }

    transaction.getTransactionHistory(data, function(err, dbResult) {
        if (err) {
            response.status(500).json(err);
        } else {
            response.json(dbResult);
        }
    });
});

// Get account balance 
router.get('/balance', authenticateToken,function(req, res) {
    const data = {
        account_id: req.query.account_id,
        card_id: req.query.card_id
    };
    if (req.user.card_id != data.card_id) {
        return res.status(403).json({ error: "Access denied" });
    }
    transaction.getBalance(data, function(err, result) {
        if (err) return res.status(400).json(err);
        res.json(result);
    });
});
module.exports = router;