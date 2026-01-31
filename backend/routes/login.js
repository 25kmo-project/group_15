const express = require('express');
const router = express.Router();
const login = require('../models/login_model');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');

router.post('/', function(request, response){
    if(request.body.card_id && request.body.pin_code){
        const card_id = request.body.card_id;
        const pin_code = request.body.pin_code;

        login.getCardIdPincode(card_id, function(err, result){
            if(err){
                console.log("Database error");
                response.json({"message":"Database error"});
            }
            else{
                if(result.length > 0){
                    bcrypt.compare(pin_code, result[0].pin_code, function(err, compareResult){
                        if(err){
                            console.log("Error checking pin:", err);
                            response.json({"message": "Error checking pin code"});
                        }
                        else if(!compareResult){
                            console.log("Pincode is not correct");
                            response.json({"message":"Pincode is not correct"});
                        }
                        else {
                            const token = generateAccessToken(card_id);
                            response.setHeader('Content-Type','application/json');
                            response.json({
                                success: true,
                                message: "Success",
                                card_id: card_id,
                                token: token
                            });
                        }
                    }); 
                }
                else {
                    console.log("Card not found");
                    response.json({"message":"Card not found"});
                }
            }
        }); 
    }
    else{
        console.log("Card id or pincode is not given");
        response.json({"message":"Card id or pincode is not given"});
    }
});

function generateAccessToken(card_id){
    return jwt.sign({card_id}, process.env.MY_TOKEN, {expiresIn: '1800s'});
}

module.exports = router;
