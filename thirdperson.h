#pragma once

class thirdperson : public singleton<thirdperson>
{
public:
	void run( ) const;
	void set_networked_angle( const QAngle networked_angle ) const;
	QAngle& get_networked_angle() const { return networked_angle; }
	void set_clientside_angle(const QAngle clientside_angle) const;
	QAngle& get_clientside_angle() const { return clientside_angle; }
private:
	mutable QAngle networked_angle;
	mutable QAngle clientside_angle;
};