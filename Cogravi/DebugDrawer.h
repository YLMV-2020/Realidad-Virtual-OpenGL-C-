#ifndef DEBUG_DRAWER_HEADER
#define DEBUG_DRAWER_HEADER

namespace Cogravi
{
	class DebugDrawer : public btIDebugDraw
	{
	public:

		// debug mode functions
		virtual void setDebugMode(int debugMode) override { m_debugMode = debugMode; }
		virtual int getDebugMode() const override { return m_debugMode; }

		// drawing functions
		//virtual void  drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override;
		//virtual void  drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

		// unused
		virtual void  reportErrorWarning(const char* warningString) override {}
		virtual void  draw3dText(const btVector3& location, const char* textString) override {}

		//void ToggleDebugFlag(int flag);

		void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix)
		{
			glUseProgram(0); // Use Fixed-function pipeline (no shaders)
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(&pViewMatrix[0][0]);
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(&pProjectionMatrix[0][0]);
		}

		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
		{
			// draws a simple line of pixels between points.

			// use the GL_LINES primitive to draw lines
			glBegin(GL_LINES);
			//glColor3f(color.x(), color.y(), color.z());
			glColor3f(col.x, col.y, col.z);
			glVertex3f(from.getX(), from.getY(), from.getZ());
			glVertex3f(to.getX(), to.getY(), to.getZ());
			glEnd();
		}

		virtual void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
		{
			// draws a line between two contact points
			btVector3 const startPoint = pointOnB;
			btVector3 const endPoint = pointOnB + normalOnB * distance;
			drawLine(startPoint, endPoint, color);
		}

		void ToggleDebugFlag(int flag) {
			// checks if a flag is set and enables/
			// disables it
			if (m_debugMode & flag) {
				// flag is enabled, so disable it
				m_debugMode = m_debugMode & (~flag);
			}
			else {
				// flag is disabled, so enable it
				m_debugMode |= flag;
			}
		}
	protected:
		int m_debugMode;

	public:
		glm::vec3 col = glm::vec3(0, 1, 0);

	};
}

#endif
