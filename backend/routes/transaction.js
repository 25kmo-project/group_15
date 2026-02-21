const express = require('express');
const router = express.Router();
const transaction = require('../models/transaction_model');
const authenticateToken = require('../middleware/auth');
const db = require('../routes/database');


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

router.post('/withdraw', authenticateToken, function(req, res) {
    const { account_id, card_id, amount } = req.body;

    // Check that the user is using their own card
    if (req.user.card_id != card_id) {
        return res.status(403).json({ message: "Security breach: You cannot access other cards" });
    }

    // Backend validation: amount must be at least 20
    const a = Number(amount);
    if (!Number.isInteger(a) || a < 20) {
        return res.status(400).json({ message: "Invalid amount: minimum 20€" });
    }

    // Backend validation: only allow amounts that can be formed with 20€ and 50€ bills
    let valid = false;
    for (let fifties = 0; fifties * 50 <= a; fifties++) {
        if ((a - fifties * 50) % 20 === 0) valid = true;
    }
    if (!valid) {
        return res.status(400).json({ message: "Invalid amount: must be 20/50 combination" });
    }

    // If all checks pass, execute the withdrawal
    transaction.withdraw({ account_id, card_id, amount: a }, function(err, result) {
        if (err) {
            const clientErrors = ['INVALID_AMOUNT', 'NOT_FOUND', 'LOCKED', 'INSUFFICIENT_FUNDS'];
            const statusCode = clientErrors.includes(err.error) ? 400 : 500;
            return res.status(statusCode).json(err);
        }
        res.json({
            message: "Withdrawal successful",
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

router.post('/transfer', authenticateToken, function(request, response) {
    // 1. check if request body exists
    if (!request.body) {
        return response.status(400).json({ error: "No request body received" });
    }
    
    const data = {
        sender_account_id: parseInt(request.body.sender_account_id),
        receiver_account_number: request.body.receiver_account_number,
        card_id: request.body.card_id, 
        amount: parseFloat(request.body.amount)
    };
    
    // 2. validate required parameters and their types
    if (!data.sender_account_id || !data.receiver_account_number || !data.card_id || isNaN(data.amount)) {
        return response.status(400).json({ error: "Missing or invalid parameters" });
    }

    // 3. validate that the card_id in the token matches the card_id in the request body
    if (Number(request.user.card_id) !== Number(data.card_id)) {
    console.log(`Unauthorized: Token(${request.user.card_id}) vs Body(${data.card_id})`);
    return response.status(403).json({ message: "UNAUTHORIZED_CARD" });
    }

    transaction.transfer(data, function(err, dbResult) {
    if (err) {
        console.error('SQL Error:', err.sqlMessage);
        console.error('Data:', data);
        return response.status(500).json({ message: err.sqlMessage || "Database Error" });
    } else {
        console.log('DB Result:', dbResult);
        if (dbResult && dbResult[0] && dbResult[0][0]) {
            response.json(dbResult[0][0]);
        } else {
            return response.json({ message: "Transfer completed" });
        }
     }
    });
});

router.get('/receipt', authenticateToken, function(req, res) {
    console.log("receipt token payload:", req.user);

  const account_id = parseInt(req.query.account_id);
  const card_id = parseInt(req.query.card_id);

  if (isNaN(account_id) || isNaN(card_id)) {
    return res.status(400).json({ error: "Valid account_id and card_id required" });
  }

  // varmista että tokenin card_id == pyydetty card_id
  if (Number(req.user.card_id) !== Number(card_id)) {
    return res.status(403).json({ error: "Access denied" });
  }

  // 1) Hae saldo ilman inquiry-logia
  transaction.getBalanceNoLog({ account_id, card_id }, function(err, bal) {
    if (err) return res.status(403).json(err);

    const session_id = req.user.session_id;
    const nowIso = new Date().toISOString();

    // jos session_id puuttuu -> palautetaan vain saldo + aika
    if (!session_id) {
      return res.json({
        title: "Tulosta kuitti",
        app: "Pankkiautomaattisovellus",
        timestamp: nowIso,
        balance: bal.balance,
        events: []
      });
    }

    //hae session login_time
    db.query(
      "SELECT login_time FROM session WHERE session_id = ? AND card_id = ?",
      [session_id, card_id],
      function(sErr, sRes) {
        if (sErr) return res.status(500).json({ error: "DB error" });
        if (!sRes || sRes.length === 0) {
          return res.json({
            title: "Tulosta kuitti",
            app: "Pankkiautomaattisovellus",
            timestamp: nowIso,
            balance: bal.balance,
            events: []
          });
        }

        const login_time = sRes[0].login_time;

        // hae tämän session tapahtumat transaction-taulusta
        // huom: jätetään INQUIRY pois, jotta pelkkä saldon katselu ei tee “käytetty palveluja”
        const evSql = `
          SELECT transaction_type, amount, transaction_date
          FROM transaction
          WHERE account_id = ?
            AND card_id = ?
            AND transaction_date >= ?
            AND transaction_type IN ('WITHDRAWAL', 'DEPOSIT', 'TRANSFER')
          ORDER BY transaction_date ASC`;

        db.query(evSql, [account_id, card_id, login_time], function(evErr, events) {
          if (evErr) return res.status(500).json({ error: "DB error" });

          return res.json({
            title: "Tulosta kuitti",
            app: "Pankkiautomaattisovellus",
            timestamp: nowIso,
            balance: bal.balance,
            events: events || []
          });
        });
      }
    );
  });
});

module.exports = router;