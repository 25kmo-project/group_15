const express = require('express');
const router = express.Router();
const login = require('../models/login_model');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const { v4: uuidv4 } = require('uuid');
const db = require('../routes/database');

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
                            return;

                        }
                        else {
                            // oikea PIN -> nollaa epäonnistuneet yritykset
                            login.resetFailedPinAttempts(card_id, function(err2){
                                if(err2){
                                    console.log("Database error (resetFailedPinAttempts):", err2);
                                    return response.status(500).json({"message":"Database error: Failed to reset attempts"});
                                }

                               const session_id = uuidv4();

db.query(
    "INSERT INTO session (session_id, card_id) VALUES (?, ?)",
    [session_id, card_id],
    function(sessErr){
        if(sessErr){
            console.log("Database error (insert session):", sessErr);
            return response.status(500).json({"message":"Database error: Failed to create session"});
        }

        // tokeniin card_id + session_id
        const token = generateAccessToken({ card_id: Number(card_id), session_id });

        //tarkistetaan kortin tyypit: debit ja credit rivit 69-77
        const accountTypes = [];
        const accountIds = [];

        for(let i = 0; i < result.length; i++){
            accountIds.push(result[i].account_id);

            if(!accountTypes.includes(result[i].account_type)){
                accountTypes.push(result[i].account_type);
            }
        }

        const user_id = result[0].user_id;

        //vastaus json-muodossa
        response.setHeader('Content-Type','application/json');
        return response.status(200).json({
            success: true,
            message: "Login successful",
            card_id: Number(card_id),
            user_id: user_id,
            account_id: accountIds,
            account_types: accountTypes,
            side_selection: accountTypes.length > 1,
            session_id: session_id,  // <-- lisätty
            token: token
        });
    }
);
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

function generateAccessToken(payload){
    return jwt.sign(payload, process.env.MY_TOKEN, {expiresIn: '1800s'});
}

module.exports = router;
