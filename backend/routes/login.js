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
                return response.json({"message":"Database error"});
            }
            else{
                if(result.length > 0){

                    // kortin status tarkistus
                    if(result[0].status && result[0].status !== 'ACTIVE'){
                        console.log("Card is not active");
                        return response.json({"message":"Card is not active"});
                    }

                    bcrypt.compare(pin_code, result[0].pin_code, function(err, compareResult){
                        if(err){
                            console.log("Error checking pin:", err);
                            return response.json({"message": "Error checking pin code"});
                        }
                        else if(!compareResult){
                            console.log("Pincode is not correct");

                            // kasvatetaan epäonnistuneet yritykset VAIN väärällä PINillä
                            login.updateFailedPinAttempts(card_id, function(err2){
                                if(err2){
                                    console.log("Database error (updateFailedPinAttempts)");
                                    return response.json({"message":"Database error"});
                                }

                                const attemptsNow = (result[0].failed_pin_attempts || 0) + 1;

                                // 3 väärää -> sulje kortti
                                if(attemptsNow >= 3){
                                    login.closeCard(card_id, function(err3){
                                        if(err3){
                                            console.log("Database error (closeCard)");
                                            return response.json({"message":"Database error"});
                                        }
                                        return response.json({"message":"Card closed: pin wrong 3 times"});
                                    });
                                } else {
                                    return response.json({"message":`Pincode is not correct (${attemptsNow}/3)`});
                                }
                            });

                        }
                        else {
                            // oikea PIN -> nollaa epäonnistuneet yritykset
                            login.resetFailedPinAttempts(card_id, function(err2){
                                if(err2){
                                    console.log("Database error (resetFailedPinAttempts)");
                                    return response.json({"message":"Database error"});
                                }

                                const token = generateAccessToken(card_id);
                                response.setHeader('Content-Type','application/json');
                                return response.json({
                                    success: true,
                                    message: "Success",
                                    card_id: card_id,
                                    token: token
                                });
                            });
                        }
                    }); 
                }
                else {
                    console.log("Card not found");
                    return response.json({"message":"Card not found"});
                }
            }
        }); 
    }
    else{
        console.log("Card id or pincode is not given");
        return response.json({"message":"Card id or pincode is not given"});
    }
});

function generateAccessToken(card_id){
    return jwt.sign({card_id}, process.env.MY_TOKEN, {expiresIn: '1800s'});
}

module.exports = router;
