const express = require('express');
const router = express.Router();
const access = require('../models/account_access_model');

// GET / kaikki account_access rivit
router.get('/', function(request, response){
    access.getAll(function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result);
        }
    });
});

// GET /:card_id kaikki oikeudet tälle kortille
router.get('/card/:card_id', function(request, response){
    access.getByCardId(request.params.card_id, function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result);
        }
    });
});

// GET /:account_id kaikki oikeudet tälle tilille
router.get('/account/:account_id', function(request, response){
    access.getByAccountId(request.params.account_id, function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result);
        }
    });
});

// GET /:card_id/:account_id hae yhden kortin ja tilin oikeudet
router.get('/:card_id/:account_id', function(request, response){
    access.getOne(request.params.card_id, request.params.account_id, function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result[0]);
        }
    });
});

// POST / lisää uusi oikeus
router.post('/', function(request, response){
    access.add(request.body, function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result);
        }
    });
});

// PUT / päivitä access_type
router.put('/:card_id/:account_id', function(request, response){
    access.update(request.params.card_id, request.params.account_id, request.body.access_type, function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result);
        }
    });
});

// DELETE / poista oikeus
router.delete('/:card_id/:account_id', function(request, response){
    access.delete(request.params.card_id, request.params.account_id, function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result);
        }
    });
});
module.exports = router;