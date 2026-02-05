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
                console.log("Database error (getCardIdPincode):", err);
                return response.status(500).json({"message":"Database error: System failed to read the card"});
            }
            else{
                if(result.length > 0){

                    // kortin status tarkistus
                    if(result[0].status && result[0].status !== 'ACTIVE'){
                        console.log("Card is not active");
                        return response.status(403).json({"message":"Card is not active"});
                    }

                    bcrypt.compare(pin_code, result[0].pin_code, function(err, compareResult){
                        if(err){
                            console.log("Error checking pin:", err);
                            return response.status(500).json({"message": "Error checking pin code"});
                        }
                        else if(!compareResult){
                            console.log("Pincode is not correct");

                            // kasvatetaan epäonnistuneet yritykset VAIN väärällä PINillä
                            login.updateFailedPinAttempts(card_id, function(err2){
                                if(err2){
                                    console.log("Database error (updateFailedPinAttempts):", err2);
                                    return response.status(500).json({"message":"Database error: Failed to update attempts"});
                                }

                                const attemptsNow = (result[0].failed_pin_attempts || 0) + 1;

                                // 3 väärää -> sulje kortti
                                if(attemptsNow >= 3){
                                    login.closeCard(card_id, function(err3){
                                        if(err3){
                                            console.log("Database error (closeCard):", err3);
                                            return response.status(500).json({"message":"Database error: Failed to close card"});
                                        }
                                        return response.status(403).json({"message":"Card blocked: Incorrect PIN entered 3 times"});
                                    });
                                } else {
                                    return response.status(401).json({"message":`Pincode is not correct (${attemptsNow}/3)`});
                                }
                            });

                        }
                        else {
                            // oikea PIN -> nollaa epäonnistuneet yritykset
                            login.resetFailedPinAttempts(card_id, function(err2){
                                if(err2){
                                    console.log("Database error (resetFailedPinAttempts):", err2);
                                    return response.status(500).json({"message":"Database error: Failed to reset attempts"});
                                }

                                const token = generateAccessToken(card_id);
                                response.setHeader('Content-Type','application/json');
                                return response.status(200).json({
                                    success: true,
                                    message: "Login successful",
                                    card_id: card_id,
                                    account_id: result[0].account_id,
                                    token: token
                                });
                            });
                        }
                    }); 
                }
                else {
                    console.log("Card not found");
                    return response.status(404).json({"message":"Card not found"});
                }
            }
        }); 
    }
    else{
        console.log("Card id or pincode is not given");
        return response.status(400).json({"message":"Card id or pincode is not given"});
    }
});

function generateAccessToken(card_id){
    return jwt.sign({card_id}, process.env.MY_TOKEN, {expiresIn: '1800s'});
}

module.exports = router;
