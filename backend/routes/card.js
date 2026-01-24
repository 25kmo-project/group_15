const express = require("express");
const router = express.Router();
const card = require("../models/card_model");
const e = require("express");

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


// Päivitä PIN (hashataan)
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
// päivitä kortti id:llä (ei PINiä) 
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