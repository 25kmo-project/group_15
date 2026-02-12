const express = require('express');
const router = express.Router();
const transaction = require('../models/transaction_model');
const authenticateToken = require('../middleware/auth');


// Get a single transaction by its ID
router.get('/id/:id',authenticateToken, function(request, response) {
    transaction.getById(request.params.id, function(err, dbResult) {
        if (err) return response.status(500).json(err);
        if (!dbResult || dbResult.length === 0) {
            return response.status(404).json({ error: "Transaction not found" });
        }
        if (dbResult[0].card_id != request.user.card_id) {
            return response.status(403).json({ error: "Access denied" });
      }
        // Returns the first (and only) result from the array
            response.json(dbResult[0]);
    });
});

// create a new withdrawal transaction
router.post('/withdraw',authenticateToken, function(req, res) {
    const { account_id, card_id, amount } = req.body;
        if (req.user.card_id != card_id) {
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
router.get('/history', authenticateToken, function(request, response) {
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
router.get('/balance', authenticateToken, function(req, res) {
    const data = {
        account_id: parseInt(req.query.account_id),
        card_id: parseInt(req.query.card_id)
    };
    if (req.user.card_id != data.card_id) {
        return res.status(403).json({ error: "Access denied" });
    }
    transaction.getBalance(data, function(err, result) {
        if (err) return res.status(400).json(err);
        res.json(result);
    });
});

// create a new deposit transaction
router.post('/deposit', authenticateToken, function(req, res) {
    const { account_id, card_id, amount } = req.body;

    if (req.user.card_id != card_id) {
        return res.status(403).json({ error: "Access denied" });
    }

    transaction.deposit({ account_id, card_id, amount }, function(err, result) {
        if (err) {
            const clientErrors = ['INVALID_AMOUNT', 'NOT_FOUND', 'LOCKED', 'UNAUTHORIZED'];
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