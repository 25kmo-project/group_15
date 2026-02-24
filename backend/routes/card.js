const express = require("express");
const router = express.Router();
const card = require("../models/card_model");

//Get all cards
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

//Get single card by id
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

//Add new card
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


//Update PIN (hashed before storing)
router.put("/:card_id/pin", function(request, response) {
  card.updatePin(request.params.card_id, request.body.pin_code, function(err, result) {
        if (err){
            response.send(err);
        }
        else {
            response.json(result);
        }
  });
});
//Update card details by id (excluding PIN)
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

//Delete card by id 
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