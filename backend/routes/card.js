const express = require("express");
const router = express.Router();
const card = require("../models/card_model");

//Hae kaikki kortit
router.get("/", function(request, response){
    card.getAll(function(err, result){
        if(err) {
            response.send (err);
        }
        else{
            response.json(result);
        }
    })
});

// hae yksi kortti id:llä  
router.get("/:card_id", function(request, response){
    card.getOne(request.params.card_id, function(err, result){
        if (err){
            response.send(err);
        } 
        else {
             response.json(result[0]);
        }
    })
});

// lisää uusi kortti
router.post("/", function(request, response){
    card.add(request.body, function(err, result){
        if (err){
            response.send(err);
        }
        else {
            response.json(result);
        }
    })
});

// päivitä kortti id:llä  
router.put("/:card_id", function(request, response){   
    card.update(request.body, request.params.card_id, function(err, result){
        if (err){
            response.send(err);
        }
        else {
            response.json(result);
        }
    })
});

// poista kortti id:llä  
router.delete("/:card_id", function(request, response){
    card.delete(request.params.card_id, function(err, result){
        if (err){
            response.send(err);
        }
        else {
            response.json(result);
        }
    });
});

module.exports = router;