const express = require('express');
const router = express.Router();
const access = require('../models/account_access_model');

//Get all account_access rows
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

//Get all access rights for a specific card
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

//Get all access rights for a specific account
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

//Get access rights for a specific card and account
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

//Add new access right
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

//Update access_type
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

//Delete access right
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